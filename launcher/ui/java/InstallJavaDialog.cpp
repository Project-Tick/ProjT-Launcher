// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, version 3.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "InstallJavaDialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "Application.h"
#include "BaseVersionList.h"
#include "FileSystem.h"
#include "Filter.h"
#include "java/core/RuntimePackage.hpp"
#include "java/download/RuntimeArchiveTask.hpp"
#include "java/download/RuntimeManifestTask.hpp"
#include "java/download/RuntimeLinkTask.hpp"
#include "meta/Index.hpp"
#include "meta/VersionList.hpp"
#include "minecraft/MinecraftInstance.h"
#include "minecraft/PackProfile.h"
#include "tasks/SequentialTask.h"
#include "ui/dialogs/CustomMessageBox.h"
#include "ui/dialogs/ProgressDialog.h"
#include "ui/java/VersionList.h"
#include "ui/widgets/PageContainer.h"
#include "ui/widgets/VersionSelectWidget.h"

class InstallJavaPage : public QWidget, public BasePage
{
  public:
	Q_OBJECT
  public:
	explicit InstallJavaPage(const QString& id, const QString& iconName, const QString& name, QWidget* parent = nullptr)
		: QWidget(parent),
		  uid(id),
		  iconName(iconName),
		  name(name)
	{
		setObjectName(QStringLiteral("VersionSelectWidget"));
		horizontalLayout = new QHBoxLayout(this);
		horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
		horizontalLayout->setContentsMargins(0, 0, 0, 0);

		majorVersionSelect = new VersionSelectWidget(this);
		majorVersionSelect->selectCurrent();
		majorVersionSelect->setEmptyString(tr("No Java versions are currently available in the meta."));
		majorVersionSelect->setEmptyErrorString(tr("Couldn't load or download the Java version lists!"));
		horizontalLayout->addWidget(majorVersionSelect, 1);

		javaVersionSelect = new VersionSelectWidget(this);
		javaVersionSelect->setEmptyString(tr("No Java versions are currently available for your OS."));
		javaVersionSelect->setEmptyErrorString(tr("Couldn't load or download the Java version lists!"));
		horizontalLayout->addWidget(javaVersionSelect, 4);
		connect(majorVersionSelect,
				&VersionSelectWidget::selectedVersionChanged,
				this,
				&InstallJavaPage::setSelectedVersion);
		connect(majorVersionSelect,
				&VersionSelectWidget::selectedVersionChanged,
				this,
				&InstallJavaPage::selectionChanged);
		connect(javaVersionSelect,
				&VersionSelectWidget::selectedVersionChanged,
				this,
				&InstallJavaPage::selectionChanged);

		QMetaObject::connectSlotsByName(this);
	}
	~InstallJavaPage()
	{
		delete horizontalLayout;
		delete majorVersionSelect;
		delete javaVersionSelect;
	}

	//! loads the list if needed.
	void initialize(projt::meta::MetaVersionList::Ptr vlist)
	{
		vlist->setAvailableRoles(
			{ BaseVersionList::JavaMajorRole, BaseVersionList::RecommendedRole, BaseVersionList::VersionPointerRole });
		majorVersionSelect->initialize(vlist.get());
	}

	void setSelectedVersion(BaseVersion::Ptr version)
	{
		auto dcast = std::dynamic_pointer_cast<projt::meta::MetaVersion>(version);
		if (!dcast)
		{
			return;
		}
		javaVersionSelect->initialize(new Java::VersionList(dcast, this));
		javaVersionSelect->selectCurrent();
	}

	QString id() const override
	{
		return uid;
	}
	QString displayName() const override
	{
		return name;
	}
	QIcon icon() const override
	{
		return QIcon::fromTheme(iconName);
	}

	void openedImpl() override
	{
		if (loaded)
			return;

		const auto versions = APPLICATION->metadataIndex()->component(uid);
		if (!versions)
			return;

		initialize(versions);
		loaded = true;
	}

	void setParentContainer(BasePageContainer* container) override
	{
		auto* pageContainer = dynamic_cast<PageContainer*>(container);
		auto* dialog		= pageContainer ? qobject_cast<QDialog*>(pageContainer->parent()) : nullptr;
		if (!dialog || !javaVersionSelect || !javaVersionSelect->view())
			return;
		connect(javaVersionSelect->view(), &QAbstractItemView::doubleClicked, dialog, &QDialog::accept);
	}

	BaseVersion::Ptr selectedVersion() const
	{
		return javaVersionSelect ? javaVersionSelect->selectedVersion() : BaseVersion::Ptr{};
	}
	void selectSearch()
	{
		javaVersionSelect->selectSearch();
	}
	void loadList()
	{
		if (majorVersionSelect)
			majorVersionSelect->loadList();
		if (javaVersionSelect)
			javaVersionSelect->loadList();
	}

  public slots:
	void setRecommendedMajors(const QStringList& majors)
	{
		m_recommended_majors = majors;
		recommendedFilterChanged();
	}
	void setRecommend(bool recommend)
	{
		m_recommend = recommend;
		recommendedFilterChanged();
	}
	void recommendedFilterChanged()
	{
		if (m_recommend)
		{
			if (majorVersionSelect)
				majorVersionSelect->setFilter(BaseVersionList::ModelRoles::JavaMajorRole,
											  Filters::equalsAny(m_recommended_majors));
		}
		else
		{
			if (majorVersionSelect)
				majorVersionSelect->setFilter(BaseVersionList::ModelRoles::JavaMajorRole, Filters::equalsAny());
		}
	}

  signals:
	void selectionChanged();

  private:
	const QString uid;
	const QString iconName;
	const QString name;
	bool loaded = false;

	QHBoxLayout* horizontalLayout			= nullptr;
	VersionSelectWidget* majorVersionSelect = nullptr;
	VersionSelectWidget* javaVersionSelect	= nullptr;

	QStringList m_recommended_majors;
	bool m_recommend = false;
};

static InstallJavaPage* pageCast(BasePage* page)
{
	auto result = dynamic_cast<InstallJavaPage*>(page);
	Q_ASSERT(result != nullptr);
	return result;
}
namespace Java
{
	QStringList getRecommendedJavaVersionsFromVersionList(projt::meta::MetaVersionList::Ptr list)
	{
		QStringList recommendedJavas;
		for (auto ver : list->allVersions())
		{
			auto major = ver->versionId();
			if (major.startsWith("java"))
			{
				major = "Java " + major.mid(4);
			}
			recommendedJavas.append(major);
		}
		return recommendedJavas;
	}

	InstallDialog::InstallDialog(const QString& uid, BaseInstance* instance, QWidget* parent)
		: QDialog(parent),
		  container(new PageContainer(this, QString(), this)),
		  buttons(new QDialogButtonBox(this))
	{
		auto layout = new QVBoxLayout(this);

		container->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
		layout->addWidget(container);

		auto buttonLayout  = new QHBoxLayout(this);
		auto refreshLayout = new QHBoxLayout(this);

		auto refreshButton = new QPushButton(tr("&Refresh"), this);
		connect(refreshButton,
				&QPushButton::clicked,
				this,
				[this]
				{
					if (auto* selectedPage = pageCast(container->selectedPage()))
						selectedPage->loadList();
				});
		refreshLayout->addWidget(refreshButton);

		auto recommendedCheckBox = new QCheckBox("Recommended", this);
		recommendedCheckBox->setCheckState(Qt::CheckState::Checked);
		connect(recommendedCheckBox,
				&QCheckBox::stateChanged,
				this,
				[this](int state)
				{
					for (BasePage* page : container->getPages())
					{
						if (auto* installPage = pageCast(page))
							installPage->setRecommend(state == Qt::Checked);
					}
				});

		refreshLayout->addWidget(recommendedCheckBox);
		buttonLayout->addLayout(refreshLayout);

		buttons->setOrientation(Qt::Horizontal);
		buttons->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
		buttons->button(QDialogButtonBox::Ok)->setText(tr("Download"));
		buttons->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
		connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
		connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
		buttonLayout->addWidget(buttons);

		layout->addLayout(buttonLayout);

		setWindowTitle(dialogTitle());
		setWindowModality(Qt::WindowModal);
		resize(840, 480);

		QStringList recommendedJavas;
		if (auto mcInst = dynamic_cast<MinecraftInstance*>(instance); mcInst)
		{
			auto mc = mcInst->getPackProfile()->getComponent("net.minecraft");
			if (mc)
			{
				auto file = mc->getVersionFile(); // no need for load as it should already be loaded
				if (file)
				{
					for (auto major : file->compatibleJavaMajors)
					{
						recommendedJavas.append(QString("Java %1").arg(major));
					}
				}
			}
		}
		else
		{
			const auto versions = APPLICATION->metadataIndex()->component("net.minecraft.java");
			if (versions)
			{
				if (versions->isLoaded())
				{
					recommendedJavas = getRecommendedJavaVersionsFromVersionList(versions);
				}
				else
				{
					auto newTask = versions->getLoadTask();
					if (newTask)
					{
						connect(newTask.get(),
								&Task::succeeded,
								this,
								[this, versions]
								{
									auto recommendedJavas = getRecommendedJavaVersionsFromVersionList(versions);
									for (BasePage* page : container->getPages())
									{
										if (auto* installPage = pageCast(page))
											installPage->setRecommendedMajors(recommendedJavas);
									}
								});
						if (!newTask->isRunning())
							newTask->start();
					}
					else
					{
						recommendedJavas = getRecommendedJavaVersionsFromVersionList(versions);
					}
				}
			}
		}
		for (BasePage* page : container->getPages())
		{
			if (page->id() == uid)
				container->selectPage(page->id());

			auto cast = pageCast(page);
			if (!cast)
				continue;
			cast->setRecommend(true);
			connect(cast, &InstallJavaPage::selectionChanged, this, [this, cast] { validate(cast); });
			if (!recommendedJavas.isEmpty())
			{
				cast->setRecommendedMajors(recommendedJavas);
			}
		}
		connect(container,
				&PageContainer::selectedPageChanged,
				this,
				[this](BasePage* previous, BasePage* selected) { validate(selected); });
		if (auto* selectedPage = pageCast(container->selectedPage()))
			selectedPage->selectSearch();
		validate(container->selectedPage());
	}

	QList<BasePage*> InstallDialog::getPages()
	{
		return {
			// Mojang
			new InstallJavaPage("net.minecraft.java", "mojang", tr("Mojang")),
			// Adoptium
			new InstallJavaPage("net.adoptium.java", "adoptium", tr("Adoptium")),
			// IBM Semeru OpenJ9
			new InstallJavaPage("com.ibm.java", "openj9", tr("IBM Semeru OpenJ9")),
			// Azul
			new InstallJavaPage("com.azul.java", "azul", tr("Azul Zulu")),
		};
	}

	QString InstallDialog::dialogTitle()
	{
		return tr("Install Java");
	}

	void InstallDialog::validate(BasePage* selected)
	{
		auto* installPage = pageCast(selected);
		buttons->button(QDialogButtonBox::Ok)
			->setEnabled(installPage
						 && !!std::dynamic_pointer_cast<projt::java::RuntimePackage>(installPage->selectedVersion()));
	}

	void InstallDialog::done(int result)
	{
		if (result == Accepted)
		{
			auto* page = pageCast(container->selectedPage());
			if (page && page->selectedVersion())
			{
				auto meta = std::dynamic_pointer_cast<projt::java::RuntimePackage>(page->selectedVersion());
				if (meta)
				{
					Task::Ptr task;
					auto final_path = FS::PathCombine(APPLICATION->javaPath(), meta->displayName);
					auto deletePath = [final_path] { FS::deletePath(final_path); };
					switch (meta->downloadKind)
					{
						case projt::java::PackageKind::Manifest:
							task = makeShared<projt::java::RuntimeManifestTask>(meta->url,
																				final_path,
																				meta->checksumType,
																				meta->checksumHash);
							break;
						case projt::java::PackageKind::Archive:
							task = makeShared<projt::java::RuntimeArchiveTask>(meta->url,
																			   final_path,
																			   meta->checksumType,
																			   meta->checksumHash);
							break;
						case projt::java::PackageKind::Unknown:
							QString error = QString(tr("Could not determine Java download type!"));
							CustomMessageBox::selectable(this, tr("Error"), error, QMessageBox::Warning)->show();
							deletePath();
					}
					if (!task)
					{
						QDialog::done(result);
						return;
					}
#if defined(Q_OS_MACOS)
					auto seq = makeShared<SequentialTask>(tr("Install Java"));
					seq->addTask(task);
					seq->addTask(makeShared<projt::java::RuntimeLinkTask>(final_path));
					task = seq;
#endif
					connect(task.get(),
							&Task::failed,
							this,
							[this, &deletePath](QString reason)
							{
								QString error = QString("Java download failed: %1").arg(reason);
								CustomMessageBox::selectable(this, tr("Error"), error, QMessageBox::Warning)->show();
								deletePath();
							});
					connect(task.get(), &Task::aborted, this, deletePath);
					ProgressDialog pg(this);
					pg.setSkipButton(true, tr("Abort"));
					pg.execWithTask(*task);
				}
				else
				{
					return;
				}
			}
			else
			{
				return;
			}
		}

		QDialog::done(result);
	}

} // namespace Java

#include "InstallJavaDialog.moc"

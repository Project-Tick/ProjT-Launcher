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
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 *
 *
 *
 *
 * ======================================================================== */

#pragma once

#include <QTimer>
#include <QWidget>

#include "ResourceDownloadTask.h"
#include "modplatform/ModIndex.h"
#include "modplatform/ResourceAPI.h"

#include "ui/pages/BasePage.h"
#include "ui/pages/modplatform/ResourceModel.h"
#include "ui/widgets/ProgressWidget.h"

namespace Ui
{
	class ResourcePage;
}

class BaseInstance;

namespace ResourceDownload
{

	class ResourceDownloadDialog;
	class ResourceModel;

	class ResourcePage : public QWidget, public BasePage
	{
		Q_OBJECT
	  public:
		using DownloadTaskPtr = shared_qobject_ptr<ResourceDownloadTask>;
		~ResourcePage() override;

		/* Affects what the user sees */
		auto displayName() const -> QString override = 0;
		auto icon() const -> QIcon override			 = 0;
		auto id() const -> QString override			 = 0;
		auto helpPage() const -> QString override	 = 0;
		bool shouldDisplay() const override			 = 0;

		/* Used internally */
		virtual auto metaEntryBase() const -> QString = 0;
		virtual auto debugName() const -> QString	  = 0;

		//: The plural version of 'resource'
		virtual inline QString resourcesString() const
		{
			return tr("resources");
		}
		//: The singular version of 'resources'
		virtual inline QString resourceString() const
		{
			return tr("resource");
		}

		/* Features this resource's page supports */
		virtual bool supportsFiltering() const = 0;

		void retranslate() override;
		void openedImpl() override;
		auto eventFilter(QObject* watched, QEvent* event) -> bool override;

		/** Get the current term in the search bar. */
		auto getSearchTerm() const -> QString;
		/** Programatically set the term in the search bar. */
		void setSearchTerm(QString);

		bool setCurrentPack(ModPlatform::IndexedPack::Ptr);
		auto getCurrentPack() const -> ModPlatform::IndexedPack::Ptr;
		auto getDialog() const -> const ResourceDownloadDialog*
		{
			return m_parentDialog;
		}
		auto getModel() const -> ResourceModel*
		{
			return m_model;
		}

	  protected:
		ResourcePage(ResourceDownloadDialog* parent, BaseInstance&);

		void addSortings();

	  public slots:
		virtual void updateUi(const QModelIndex& index);
		virtual void updateSelectionButton();
		virtual void versionListUpdated(const QModelIndex& index);

		void addResourceToDialog(ModPlatform::IndexedPack::Ptr, ModPlatform::IndexedVersion&);
		void removeResourceFromDialog(const QString& pack_name);
		virtual void removeResourceFromPage(const QString& name);
		virtual void addResourceToPage(ModPlatform::IndexedPack::Ptr,
									   ModPlatform::IndexedVersion&,
									   std::shared_ptr<ResourceFolderModel>);

		virtual void modelReset();

		QList<DownloadTaskPtr> selectedPacks()
		{
			return m_model ? m_model->selectedPacks() : QList<DownloadTaskPtr>{};
		}
		bool hasSelectedPacks()
		{
			return m_model && !m_model->selectedPacks().isEmpty();
		}

		virtual void openProject(QVariant projectID);

	  protected slots:
		virtual void triggerSearch() = 0;

		void onSelectionChanged(QModelIndex first, QModelIndex second);
		void onVersionSelectionChanged(int index);
		void onResourceSelected();
		void onResourceToggle(const QModelIndex& index);

		/** Associates regex expressions to pages in the order they're given in the map. */
		virtual QMap<QString, QString> urlHandlers() const = 0;
		virtual void openUrl(const QUrl&);

	  public:
		BaseInstance& m_baseInstance;

	  protected:
		Ui::ResourcePage* m_ui;

		ResourceDownloadDialog* m_parentDialog = nullptr;
		ResourceModel* m_model				   = nullptr;

		int m_selectedVersionIndex = -1;

		ProgressWidget m_fetchProgress;

		// Used to do instant searching with a delay to cache quick changes
		QTimer m_searchTimer;

		bool m_doNotJumpToMod = false;

		QSet<int> m_enableQueue;
	};

} // namespace ResourceDownload

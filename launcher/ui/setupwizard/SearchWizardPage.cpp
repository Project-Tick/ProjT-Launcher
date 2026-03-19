// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team

#include "SearchWizardPage.h"

#include <QComboBox>
#include <QLabel>
#include <QVBoxLayout>

#include "Application.h"
#include "BuildConfig.h"
#include "ui/widgets/HubSearchProvider.h"

SearchWizardPage::SearchWizardPage(QWidget* parent) : BaseWizardPage(parent)
{
	setObjectName(QStringLiteral("searchPage"));

	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);

	m_descriptionLabel = new QLabel(this);
	m_descriptionLabel->setWordWrap(true);

	m_providerCombo = new QComboBox(this);
	for (const auto& provider : hubSearchProviders())
	{
		m_providerCombo->addItem(provider.displayName, provider.id);
	}

	const QString currentProvider =
		APPLICATION->settings() ? normalizedHubSearchProviderId(APPLICATION->settings()->get("HubSearchEngine").toString())
								: defaultHubSearchProviderId();
	const int currentIndex = m_providerCombo->findData(currentProvider);
	m_providerCombo->setCurrentIndex(currentIndex >= 0 ? currentIndex : 0);

	layout->addWidget(m_descriptionLabel);
	layout->addWidget(m_providerCombo);
	layout->addStretch(1);

	retranslate();
}

SearchWizardPage::~SearchWizardPage() = default;

bool SearchWizardPage::validatePage()
{
	if (APPLICATION->settings())
	{
		APPLICATION->settings()->set("HubSearchEngine", m_providerCombo->currentData().toString());
	}
	return true;
}

void SearchWizardPage::retranslate()
{
	setTitle(tr("Search"));
	setSubTitle(tr("Choose the default search engine for Launcher Hub."));
	m_descriptionLabel->setText(tr("When you type plain text like \"How to install Linux?\" into the Hub address bar, %1 will search with this provider by default.")
								 .arg(BuildConfig.LAUNCHER_DISPLAYNAME));
}

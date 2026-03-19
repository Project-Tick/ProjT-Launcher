// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
#pragma once

#include "BaseWizardPage.h"

class QComboBox;
class QLabel;

class SearchWizardPage : public BaseWizardPage
{
	Q_OBJECT

  public:
	explicit SearchWizardPage(QWidget* parent = nullptr);
	~SearchWizardPage() override;

	bool validatePage() override;

  protected:
	void retranslate() override;

  private:
	QLabel* m_descriptionLabel = nullptr;
	QComboBox* m_providerCombo = nullptr;
};

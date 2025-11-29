// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2025 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *  ProjT Launcher - Minecraft Launcher
 *  Copyright (C) 2025 Project Tick
 *
 *  This file is part of ProjT Launcher and is licensed under
 *  the GNU General Public License version 3 or later.
 *
 *  If this file includes work from previous open-source projects,
 *  their original copyright and license notices are preserved below.
 *
 * === Upstream License Block (Do Not Modify) ==============================
 *
 * // SPDX-License-Identifier: GPL-3.0-only
 *
 *  Prism Launcher - Minecraft Launcher
 *  Copyright (c) 2022 Jamie Mansfield <jmansfield@cadixdev.org>
 *  Copyright (C) 2022 TheKodeToad <TheKodeToad@proton.me>
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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * This file incorporates work covered by the following copyright and
 * permission notice:
 *
 *      Copyright 2013-2021 MultiMC Contributors
 *
 *      Licensed under the Apache License, Version 2.0 (the "License");
 *      you may not use this file except in compliance with the License.
 *      You may obtain a copy of the License at
 *
 *          http://www.apache.org/licenses/LICENSE-2.0
 *
 *      Unless required by applicable law or agreed to in writing, software
 *      distributed under the License is distributed on an "AS IS" BASIS,
 *      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *      See the License for the specific language governing permissions and
 *      limitations under the License.
 *
 * ======================================================================== */

#include "OtherLogsPage.h"
#include "ui_OtherLogsPage.h"

#include <QApplication>
#include <QMessageBox>
#include <QShortcut>
#include <QTextDocument>

#include "launch/LogModel.h"
#include "ui/GuiUtil.h"
#include "ui/themes/ThemeManager.h"
#include "viewmodels/LogsViewModel.h"

OtherLogsPage::OtherLogsPage(QString id, QString displayName, QString helpPage, InstancePtr instance, QWidget* parent)
    : QWidget(parent)
    , m_id(std::move(id))
    , m_displayName(std::move(displayName))
    , m_helpPage(std::move(helpPage))
    , ui(new Ui::OtherLogsPage)
    , m_currentFile(QString())
    , m_logsViewModel(nullptr)
{
    Q_UNUSED(instance);
    ui->setupUi(this);

    setControlsEnabled(false);

    auto findShortcut = new QShortcut(QKeySequence(QKeySequence::Find), this);
    connect(findShortcut, &QShortcut::activated, this, &OtherLogsPage::findActivated);

    auto findNextShortcut = new QShortcut(QKeySequence(QKeySequence::FindNext), this);
    connect(findNextShortcut, &QShortcut::activated, this, &OtherLogsPage::findNextActivated);

    auto findPreviousShortcut = new QShortcut(QKeySequence(QKeySequence::FindPrevious), this);
    connect(findPreviousShortcut, &QShortcut::activated, this, &OtherLogsPage::findPreviousActivated);

    connect(ui->searchBar, &QLineEdit::returnPressed, this, &OtherLogsPage::on_findButton_clicked);
}

OtherLogsPage::~OtherLogsPage()
{
    delete ui;
}

void OtherLogsPage::setLogsViewModel(LogsViewModel* viewModel)
{
    if (m_logsViewModel == viewModel) {
        return;
    }
    if (m_logsViewModel) {
        disconnect(m_logsViewModel, nullptr, this, nullptr);
    }
    m_logsViewModel = viewModel;
    if (!m_logsViewModel) {
        return;
    }

    connect(m_logsViewModel, &LogsViewModel::logListChanged, this, &OtherLogsPage::populateSelectLogBox);
    connect(m_logsViewModel, &LogsViewModel::selectedLogChanged, this, [this]() {
        const QString selected = m_logsViewModel->selectedLog();
        const int index = ui->selectLogBox->findText(selected);
        if (index >= 0 && ui->selectLogBox->currentIndex() != index) {
            ui->selectLogBox->blockSignals(true);
            ui->selectLogBox->setCurrentIndex(index);
            ui->selectLogBox->blockSignals(false);
        }
        m_currentFile = selected;
        updateViewModelCategory();
    });
    connect(m_logsViewModel, &LogsViewModel::logModelChanged, this, [this]() {
        if (!m_logsViewModel) {
            return;
        }
        ui->text->setModel(m_logsViewModel->proxyModel());
        ui->text->scrollToBottom();
        modelStateToUI();
        setControlsEnabled(true);
    });
    connect(m_logsViewModel, &LogsViewModel::logTextChanged, this, [this]() {
        if (m_logsViewModel) {
            ui->text->setPlainText(m_logsViewModel->logText());
        }
    });

    populateSelectLogBox();
    ui->text->setModel(m_logsViewModel->proxyModel());
    modelStateToUI();
}

void OtherLogsPage::retranslate()
{
    ui->retranslateUi(this);
}

void OtherLogsPage::openedImpl()
{
    populateSelectLogBox();
}

void OtherLogsPage::closedImpl() {}

void OtherLogsPage::populateSelectLogBox()
{
    if (!m_logsViewModel) {
        return;
    }

    ui->selectLogBox->blockSignals(true);
    ui->selectLogBox->clear();
    ui->selectLogBox->addItems(m_logsViewModel->logList());
    ui->selectLogBox->blockSignals(false);

    const QString selected = m_logsViewModel->selectedLog();
    int index = ui->selectLogBox->findText(selected);
    if (index == -1 && ui->selectLogBox->count() > 0) {
        index = 0;
    }
    if (index >= 0) {
        ui->selectLogBox->setCurrentIndex(index);
        m_currentFile = ui->selectLogBox->itemText(index);
        setControlsEnabled(true);
        reload();
    } else {
        setControlsEnabled(false);
        m_currentFile.clear();
    }
    updateViewModelCategory();
}

void OtherLogsPage::on_selectLogBox_currentIndexChanged(const int index)
{
    if (!m_logsViewModel || index < 0) {
        return;
    }
    m_currentFile = ui->selectLogBox->itemText(index);
    m_logsViewModel->setSelectedLog(m_currentFile);
    reload();
}

void OtherLogsPage::on_btnReload_clicked()
{
    reload();
}

void OtherLogsPage::reload()
{
    if (!m_logsViewModel) {
        return;
    }
    m_logsViewModel->loadLogs(m_currentFile);
    if (auto proxy = m_logsViewModel->proxyModel()) {
        ui->text->setModel(proxy);
        ui->text->scrollToBottom();
    }
    modelStateToUI();
}

void OtherLogsPage::on_btnPaste_clicked()
{
    QString name = m_currentFile.isEmpty() ? displayName() : m_currentFile;
    GuiUtil::uploadPaste(name, ui->text->toPlainText(), this);
}

void OtherLogsPage::on_btnCopy_clicked()
{
    GuiUtil::setClipboardText(ui->text->toPlainText());
}

void OtherLogsPage::on_btnBottom_clicked()
{
    ui->text->scrollToBottom();
}

void OtherLogsPage::on_trackLogCheckbox_clicked(bool checked)
{
    if (m_logsViewModel) {
        m_logsViewModel->setSuspended(!checked);
    }
}

void OtherLogsPage::on_btnDelete_clicked()
{
    if (!m_logsViewModel) {
        return;
    }
    // Retain existing UX by clearing current log contents through the VM.
    if (QMessageBox::question(this, tr("Confirm Deletion"),
                              tr("You are about to clear \"%1\" from the viewer.\n"
                                 "Are you sure?")
                                  .arg(m_currentFile.isEmpty() ? tr("current log") : m_currentFile),
                              QMessageBox::Yes, QMessageBox::No) == QMessageBox::No) {
        return;
    }
    m_logsViewModel->clearLogs(m_currentFile);
    ui->text->clear();
}

void OtherLogsPage::on_btnClean_clicked()
{
    if (!m_logsViewModel) {
        return;
    }
    m_logsViewModel->clearLogs();
    ui->text->clear();
}

void OtherLogsPage::on_wrapCheckbox_clicked(bool checked)
{
    if (m_logsViewModel) {
        m_logsViewModel->setWrapLines(checked);
    }
    ui->text->setWordWrap(checked);
    ui->text->scrollToBottom();
}

void OtherLogsPage::on_colorCheckbox_clicked(bool checked)
{
    if (m_logsViewModel) {
        m_logsViewModel->setColorLines(checked);
    }
    ui->text->setColorLines(checked);
    ui->text->scrollToBottom();
}

void OtherLogsPage::setControlsEnabled(const bool enabled)
{
    ui->btnDelete->setEnabled(enabled);
    ui->btnClean->setEnabled(enabled);
    ui->btnReload->setEnabled(enabled);
    ui->btnCopy->setEnabled(enabled);
    ui->btnPaste->setEnabled(enabled);
    ui->text->setEnabled(enabled);
    ui->trackLogCheckbox->setEnabled(enabled);
}

void OtherLogsPage::syncViewModel()
{
    if (!m_logsViewModel) {
        return;
    }
    ui->text->setModel(m_logsViewModel->proxyModel());
    modelStateToUI();
    populateSelectLogBox();
}

void OtherLogsPage::updateViewModelCategory() const
{
    if (!m_logsViewModel) {
        return;
    }
    m_logsViewModel->setCategory(currentCategoryLabel());
}

QString OtherLogsPage::currentCategoryLabel() const
{
    return m_currentFile.isEmpty() ? m_displayName : m_currentFile;
}

void OtherLogsPage::on_findButton_clicked()
{
    auto modifiers = QApplication::keyboardModifiers();
    bool reverse = modifiers & Qt::ShiftModifier;
    ui->text->findNext(ui->searchBar->text(), reverse);
}

void OtherLogsPage::findNextActivated()
{
    ui->text->findNext(ui->searchBar->text(), false);
}

void OtherLogsPage::findPreviousActivated()
{
    ui->text->findNext(ui->searchBar->text(), true);
}

void OtherLogsPage::findActivated()
{
    if (!ui->searchBar->hasFocus()) {
        ui->searchBar->setFocus();
        ui->searchBar->selectAll();
    }
}

void OtherLogsPage::modelStateToUI()
{
    if (!m_logsViewModel || !m_logsViewModel->logModel()) {
        return;
    }
    auto* logModel = qobject_cast<LogModel*>(m_logsViewModel->logModel());
    if (!logModel) {
        return;
    }
    ui->text->setWordWrap(logModel->wrapLines());
    ui->wrapCheckbox->setCheckState(logModel->wrapLines() ? Qt::Checked : Qt::Unchecked);
    ui->text->setColorLines(logModel->colorLines());
    ui->colorCheckbox->setCheckState(logModel->colorLines() ? Qt::Checked : Qt::Unchecked);
    ui->trackLogCheckbox->setCheckState(logModel->suspended() ? Qt::Unchecked : Qt::Checked);
}

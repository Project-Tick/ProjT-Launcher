// SPDX-License-Identifier: GPL-3.0-only AND Apache-2.0
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

/* === Upstream License Block (Do Not Modify) ==============================

   Licensed under the Apache-2.0 license.
   See README.md for details.

========================================================================== */

#include "ProgressWidget.h"
#include <QEventLoop>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

#include "tasks/Task.h"

ProgressWidget::ProgressWidget(QWidget* parent, bool show_label) : QWidget(parent)
{
	auto* layout = new QVBoxLayout(this);

	if (show_label)
	{
		m_label = new QLabel(this);
		m_label->setWordWrap(true);
		layout->addWidget(m_label);
	}

	m_bar = new QProgressBar(this);
	m_bar->setMinimum(0);
	m_bar->setMaximum(100);
	layout->addWidget(m_bar);

	setLayout(layout);
}

void ProgressWidget::reset()
{
	m_bar->reset();
}

void ProgressWidget::progressFormat(QString format)
{
	if (format.isEmpty())
		m_bar->setTextVisible(false);
	else
		m_bar->setFormat(format);
}

void ProgressWidget::watch(Task* task)
{
	if (!task)
		return;

	if (m_task)
		disconnect(m_task, nullptr, this, nullptr);

	m_task = task;

	connect(m_task, &Task::finished, this, &ProgressWidget::handleTaskFinish);
	connect(m_task, &Task::status, this, &ProgressWidget::handleTaskStatus);
	connect(m_task, &Task::details, this, &ProgressWidget::handleTaskStatus);
	connect(m_task, &Task::progress, this, &ProgressWidget::handleTaskProgress);
	connect(m_task, &Task::destroyed, this, &ProgressWidget::taskDestroyed);

	if (m_task->isRunning())
		show();
	else
		connect(m_task, &Task::started, this, &ProgressWidget::show);
}

void ProgressWidget::start(Task* task)
{
	watch(task);
	if (!m_task->isRunning())
		QMetaObject::invokeMethod(m_task, "start", Qt::QueuedConnection);
}

bool ProgressWidget::exec(std::shared_ptr<Task> task)
{
	QEventLoop loop;

	connect(task.get(), &Task::finished, &loop, &QEventLoop::quit);

	start(task.get());

	if (task->isRunning())
		loop.exec();

	return task->wasSuccessful();
}

void ProgressWidget::show()
{
	setHidden(false);
}
void ProgressWidget::hide()
{
	setHidden(true);
}

void ProgressWidget::handleTaskFinish()
{
	if (!m_task->wasSuccessful() && m_label)
		m_label->setText(m_task->failReason());

	if (m_hide_if_inactive)
		hide();
}
void ProgressWidget::handleTaskStatus(const QString& status)
{
	if (m_label)
		m_label->setText(status);
}
void ProgressWidget::handleTaskProgress(qint64 current, qint64 total)
{
	m_bar->setMaximum(total);
	m_bar->setValue(current);
}
void ProgressWidget::taskDestroyed()
{
	m_task = nullptr;
}

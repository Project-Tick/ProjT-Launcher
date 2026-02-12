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

#pragma once

#include <QWidget>
#include <memory>

class Task;
class QProgressBar;
class QLabel;

class ProgressWidget : public QWidget
{
	Q_OBJECT
  public:
	explicit ProgressWidget(QWidget* parent = nullptr, bool show_label = true);

	/** Whether to hide the widget automatically if it's watching no running task. */
	void hideIfInactive(bool hide)
	{
		m_hide_if_inactive = hide;
	}

	/** Reset the displayed progress to 0 */
	void reset();

	/** The text that shows up in the middle of the progress bar.
	 *  By default it's '%p%', with '%p' being the total progress in percentage.
	 */
	void progressFormat(QString);

  public slots:
	/** Watch the progress of a task. */
	void watch(Task* task);

	/** Watch the progress of a task, and start it if needed */
	void start(Task* task);

	/** Blocking way of waiting for a task to finish. */
	bool exec(std::shared_ptr<Task> task);

	/** Un-hide the widget if needed. */
	void show();

	/** Make the widget invisible. */
	void hide();

  private slots:
	void handleTaskFinish();
	void handleTaskStatus(const QString& status);
	void handleTaskProgress(qint64 current, qint64 total);
	void taskDestroyed();

  private:
	QLabel* m_label		= nullptr;
	QProgressBar* m_bar = nullptr;
	Task* m_task		= nullptr;

	bool m_hide_if_inactive = false;
};

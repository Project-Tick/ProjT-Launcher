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
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <QWidget>
#include <tasks/Task.h>

class LogUploadTask : public Task
{
	Q_OBJECT

  public:
	explicit LogUploadTask(const QString& name, const QString& logContent, QWidget* parentWidget = nullptr);
	~LogUploadTask() override = default;

	QString getUploadedUrl() const { return m_uploadedUrl; }

  protected:
	void executeTask() override;

  private slots:
	void onUploadSucceeded();
	void onUploadFailed(QString reason);

  private:
	QString m_name;
	QString m_logContent;
	QWidget* m_parentWidget;
	QString m_uploadedUrl;
	Task::Ptr m_uploadJob;
};

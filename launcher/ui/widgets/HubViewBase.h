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
#pragma once

#include <QUrl>
#include <QWidget>

class HubViewBase : public QWidget
{
	Q_OBJECT

  public:
	explicit HubViewBase(QWidget* parent = nullptr) : QWidget(parent)
	{}
	~HubViewBase() override = default;

	virtual void setUrl(const QUrl& url) = 0;
	virtual QUrl url() const			 = 0;
	virtual bool canGoBack() const		 = 0;
	virtual bool canGoForward() const	 = 0;
	virtual void setActive(bool active)
	{
		Q_UNUSED(active);
	}

  public slots:
	virtual void back()	   = 0;
	virtual void forward() = 0;
	virtual void reload()  = 0;

  signals:
	void titleChanged(const QString& title);
	void urlChanged(const QUrl& url);
	void loadFinished(bool ok);
	void navigationStateChanged();
	void newTabRequested(const QUrl& url);
};

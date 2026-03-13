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

#include "FallbackHubView.h"

#include <QDesktopServices>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

FallbackHubView::FallbackHubView(const QString& title, QWidget* parent) : HubViewBase(parent)
{
	auto* layout = new QVBoxLayout(this);
	layout->setContentsMargins(24, 24, 24, 24);
	layout->setSpacing(12);

	auto* titleLabel = new QLabel(title, this);
	titleLabel->setWordWrap(true);
	titleLabel->setStyleSheet(QStringLiteral("color: #ffffff; font-size: 18px; font-weight: 700;"));

	m_urlLabel = new QLabel(this);
	m_urlLabel->setWordWrap(true);
	m_urlLabel->setStyleSheet(QStringLiteral("color: #9bb0cc;"));

	m_openButton = new QPushButton(tr("Open in browser"), this);
	connect(m_openButton, &QPushButton::clicked, this, &FallbackHubView::reload);

	layout->addWidget(titleLabel);
	layout->addWidget(m_urlLabel);
	layout->addWidget(m_openButton, 0, Qt::AlignLeft);
	layout->addStretch(1);
}

void FallbackHubView::setUrl(const QUrl& url)
{
	m_url = url;
	m_urlLabel->setText(url.toString());
	m_openButton->setEnabled(url.isValid());
	emit urlChanged(m_url);
	emit titleChanged(m_url.isValid() ? m_url.host() : tr("Open in Browser"));
	emit navigationStateChanged();
	if (m_url.isValid())
	{
		openCurrentUrl();
	}
	emit loadFinished(m_url.isValid());
}

QUrl FallbackHubView::url() const
{
	return m_url;
}

bool FallbackHubView::canGoBack() const
{
	return false;
}

bool FallbackHubView::canGoForward() const
{
	return false;
}

void FallbackHubView::back()
{}

void FallbackHubView::forward()
{}

void FallbackHubView::reload()
{
	openCurrentUrl();
	emit loadFinished(m_url.isValid());
}

void FallbackHubView::openCurrentUrl() const
{
	if (m_url.isValid())
	{
		QDesktopServices::openUrl(m_url);
	}
}

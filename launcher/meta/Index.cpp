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

#include "Index.hpp"

#include <QEventLoop>

#include "JsonFormat.hpp"
#include "QObjectPtr.h"
#include "tasks/SequentialTask.h"

namespace projt::meta
{

	MetaIndex::MetaIndex(QObject* parent) : QAbstractListModel(parent)
	{}

	MetaIndex::MetaIndex(const QList<MetaVersionList::Ptr>& components, QObject* parent)
		: QAbstractListModel(parent),
		  m_components(components)
	{
		for (int i = 0; i < m_components.size(); ++i)
		{
			const auto& comp = m_components.at(i);
			m_componentIndex.insert(comp->uid(), comp);
			bindComponentSignals(i, comp);
		}
	}

	QVariant MetaIndex::data(const QModelIndex& idx, int role) const
	{
		if (!idx.isValid() || idx.row() < 0 || idx.row() >= m_components.size())
			return QVariant();

		const MetaVersionList::Ptr& comp = m_components.at(idx.row());

		switch (role)
		{
			case Qt::DisplayRole: return (idx.column() == 0) ? comp->displayName() : QVariant();
			case ComponentUidRole: return comp->uid();
			case ComponentNameRole: return comp->displayName();
			case ComponentListRole: return QVariant::fromValue(comp);
			default: return QVariant();
		}
	}

	int MetaIndex::rowCount(const QModelIndex& parent) const
	{
		return parent.isValid() ? 0 : m_components.size();
	}

	int MetaIndex::columnCount(const QModelIndex& parent) const
	{
		return parent.isValid() ? 0 : 1;
	}

	QVariant MetaIndex::headerData(int section, Qt::Orientation orientation, int role) const
	{
		if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section == 0)
			return tr("Component");
		return QVariant();
	}

	void MetaIndex::loadFromJson(const QJsonObject& json)
	{
		loadIndexFromJson(json, this);
	}

	bool MetaIndex::hasComponent(const QString& uid) const
	{
		return m_componentIndex.contains(uid);
	}

	MetaVersionList::Ptr MetaIndex::component(const QString& uid)
	{
		MetaVersionList::Ptr existing = m_componentIndex.value(uid);
		if (existing)
			return existing;

		auto newList = std::make_shared<MetaVersionList>(uid);
		int row		 = m_components.size();
		beginInsertRows(QModelIndex(), row, row);
		m_componentIndex.insert(uid, newList);
		m_components.append(newList);
		bindComponentSignals(row, newList);
		endInsertRows();

		return newList;
	}

	MetaVersion::Ptr MetaIndex::version(const QString& componentUid, const QString& versionId)
	{
		return component(componentUid)->getOrCreateVersion(versionId);
	}

	void MetaIndex::mergeWith(const std::shared_ptr<MetaIndex>& other)
	{
		const auto& incoming = other->m_components;

		if (m_components.isEmpty())
		{
			beginResetModel();
			m_components = incoming;

			for (int i = 0; i < incoming.size(); ++i)
			{
				const auto& comp = incoming.at(i);
				m_componentIndex.insert(comp->uid(), comp);
				bindComponentSignals(i, comp);
			}

			endResetModel();
			return;
		}

		for (const auto& incomingComp : incoming)
		{
			MetaVersionList::Ptr existing = m_componentIndex.value(incomingComp->uid());

			if (existing)
			{
				existing->updateFromIndex(incomingComp);
			}
			else
			{
				int row = m_components.size();
				beginInsertRows(QModelIndex(), row, row);

				bindComponentSignals(row, incomingComp);
				m_components.append(incomingComp);
				m_componentIndex.insert(incomingComp->uid(), incomingComp);

				endInsertRows();
			}
		}
	}

	void MetaIndex::bindComponentSignals(int row, const MetaVersionList::Ptr& comp)
	{
		connect(comp.get(),
				&MetaVersionList::displayNameChanged,
				this,
				[this, row]() { emit dataChanged(index(row), index(row), { Qt::DisplayRole, ComponentNameRole }); });
	}

	Task::Ptr MetaIndex::loadVersionTask(const QString& componentUid,
										 const QString& versionId,
										 Net::Mode mode,
										 bool forceRefresh)
	{
		if (mode == Net::Mode::Offline)
			return version(componentUid, versionId)->createLoadTask(mode);

		auto compList = component(componentUid);
		auto seq	  = makeShared<SequentialTask>(tr("Load metadata for %1:%2").arg(componentUid, versionId));

		if (state() != MetaEntity::State::Synchronized || forceRefresh)
			seq->addTask(this->createLoadTask(mode));

		seq->addTask(compList->createLoadTask(mode));
		seq->addTask(compList->getOrCreateVersion(versionId)->createLoadTask(mode));

		return seq;
	}

	MetaVersion::Ptr MetaIndex::loadVersionBlocking(const QString& componentUid, const QString& versionId)
	{
		QEventLoop loop;
		auto task = loadVersionTask(componentUid, versionId);
		connect(task.get(), &Task::finished, &loop, &QEventLoop::quit);
		task->start();
		loop.exec();

		return version(componentUid, versionId);
	}

} // namespace projt::meta

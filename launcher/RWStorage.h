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
#include <QMap>
#include <QReadLocker>
#include <QSet>
#include <QWriteLocker>

template <typename K, typename V>
class RWStorage
{
  public:
	void add(K key, V value)
	{
		QWriteLocker l(&lock);
		cache[key] = value;
		stale_entries.remove(key);
	}
	V get(K key)
	{
		QReadLocker l(&lock);
		if (cache.contains(key))
		{
			return cache[key];
		}
		else
			return V();
	}
	bool get(K key, V& value)
	{
		QReadLocker l(&lock);
		if (cache.contains(key))
		{
			value = cache[key];
			return true;
		}
		else
			return false;
	}
	bool has(K key)
	{
		QReadLocker l(&lock);
		return cache.contains(key);
	}
	bool stale(K key)
	{
		QReadLocker l(&lock);
		if (!cache.contains(key))
			return true;
		return stale_entries.contains(key);
	}
	void setStale(K key)
	{
		QWriteLocker l(&lock);
		if (cache.contains(key))
		{
			stale_entries.insert(key);
		}
	}
	void clear()
	{
		QWriteLocker l(&lock);
		cache.clear();
		stale_entries.clear();
	}

  private:
	QReadWriteLock lock;
	QMap<K, V> cache;
	QSet<K> stale_entries;
};

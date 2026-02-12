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

#include <QCoreApplication>
#include <QDebug>
#include <QPixmapCache>
#include <QThread>
#include <QTime>
#include <limits>

#define GET_TYPE()                                                                                                     \
	Qt::ConnectionType type;                                                                                           \
	if (QThread::currentThread() != QCoreApplication::instance()->thread())                                            \
		type = Qt::BlockingQueuedConnection;                                                                           \
	else                                                                                                               \
		type = Qt::DirectConnection;

#define DEFINE_FUNC_NO_PARAM(NAME, RET_TYPE)                                                                           \
	static RET_TYPE NAME()                                                                                             \
	{                                                                                                                  \
		RET_TYPE ret;                                                                                                  \
		GET_TYPE()                                                                                                     \
		QMetaObject::invokeMethod(s_instance, "_" #NAME, type, Q_RETURN_ARG(RET_TYPE, ret));                           \
		return ret;                                                                                                    \
	}
#define DEFINE_FUNC_ONE_PARAM(NAME, RET_TYPE, PARAM_1_TYPE)                                                            \
	static RET_TYPE NAME(PARAM_1_TYPE p1)                                                                              \
	{                                                                                                                  \
		RET_TYPE ret;                                                                                                  \
		GET_TYPE()                                                                                                     \
		QMetaObject::invokeMethod(s_instance, "_" #NAME, type, Q_RETURN_ARG(RET_TYPE, ret), Q_ARG(PARAM_1_TYPE, p1));  \
		return ret;                                                                                                    \
	}
#define DEFINE_FUNC_TWO_PARAM(NAME, RET_TYPE, PARAM_1_TYPE, PARAM_2_TYPE)                                              \
	static RET_TYPE NAME(PARAM_1_TYPE p1, PARAM_2_TYPE p2)                                                             \
	{                                                                                                                  \
		RET_TYPE ret;                                                                                                  \
		GET_TYPE()                                                                                                     \
		QMetaObject::invokeMethod(s_instance,                                                                          \
								  "_" #NAME,                                                                           \
								  type,                                                                                \
								  Q_RETURN_ARG(RET_TYPE, ret),                                                         \
								  Q_ARG(PARAM_1_TYPE, p1),                                                             \
								  Q_ARG(PARAM_2_TYPE, p2));                                                            \
		return ret;                                                                                                    \
	}

/** A wrapper around QPixmapCache with thread affinity with the main thread.
 */
class PixmapCache final : public QObject
{
	Q_OBJECT

  public:
	PixmapCache(QObject* parent) : QObject(parent)
	{}
	~PixmapCache() override = default;

	static PixmapCache& instance()
	{
		return *s_instance;
	}
	static void setInstance(PixmapCache* i)
	{
		s_instance = i;
	}

  public:
	DEFINE_FUNC_NO_PARAM(cacheLimit, int)
	DEFINE_FUNC_NO_PARAM(clear, bool)
	DEFINE_FUNC_TWO_PARAM(find, bool, const QString&, QPixmap*)
	DEFINE_FUNC_TWO_PARAM(find, bool, const QPixmapCache::Key&, QPixmap*)
	DEFINE_FUNC_TWO_PARAM(insert, bool, const QString&, const QPixmap&)
	DEFINE_FUNC_ONE_PARAM(insert, QPixmapCache::Key, const QPixmap&)
	DEFINE_FUNC_ONE_PARAM(remove, bool, const QString&)
	DEFINE_FUNC_ONE_PARAM(remove, bool, const QPixmapCache::Key&)
	DEFINE_FUNC_TWO_PARAM(replace, bool, const QPixmapCache::Key&, const QPixmap&)
	DEFINE_FUNC_ONE_PARAM(setCacheLimit, bool, int)
	DEFINE_FUNC_NO_PARAM(markCacheMissByEviciton, bool)
	DEFINE_FUNC_ONE_PARAM(setFastEvictionThreshold, bool, int)

	// NOTE: Every function returns something non-void to simplify the macros.
  private slots:
	int _cacheLimit()
	{
		return QPixmapCache::cacheLimit();
	}
	bool _clear()
	{
		QPixmapCache::clear();
		return true;
	}
	bool _find(const QString& key, QPixmap* pixmap)
	{
		return QPixmapCache::find(key, pixmap);
	}
	bool _find(const QPixmapCache::Key& key, QPixmap* pixmap)
	{
		return QPixmapCache::find(key, pixmap);
	}
	bool _insert(const QString& key, const QPixmap& pixmap)
	{
		return QPixmapCache::insert(key, pixmap);
	}
	QPixmapCache::Key _insert(const QPixmap& pixmap)
	{
		return QPixmapCache::insert(pixmap);
	}
	bool _remove(const QString& key)
	{
		QPixmapCache::remove(key);
		return true;
	}
	bool _remove(const QPixmapCache::Key& key)
	{
		QPixmapCache::remove(key);
		return true;
	}
	bool _replace(const QPixmapCache::Key& key, const QPixmap& pixmap)
	{
		return QPixmapCache::replace(key, pixmap);
	}
	bool _setCacheLimit(int n)
	{
		QPixmapCache::setCacheLimit(n);
		return true;
	}

	/**
	 *  Mark that a cache miss occurred because of a eviction if too many of these occur too fast the cache size is
	 * increased
	 * @return if the cache size was increased
	 */
	bool _markCacheMissByEviciton()
	{
		static constexpr uint maxCache = static_cast<uint>(std::numeric_limits<int>::max()) / 4;
		static constexpr uint step	   = 10240;
		static constexpr int oneSecond = 1000;

		auto now = QTime::currentTime();
		if (!m_last_cache_miss_by_eviciton.isNull())
		{
			auto diff = m_last_cache_miss_by_eviciton.msecsTo(now);
			if (diff < oneSecond)
			{ // less than a second ago
				++m_consecutive_fast_evicitons;
			}
			else
			{
				m_consecutive_fast_evicitons = 0;
			}
		}
		m_last_cache_miss_by_eviciton = now;
		if (m_consecutive_fast_evicitons >= m_consecutive_fast_evicitons_threshold)
		{
			// increase the cache size
			uint newSize = _cacheLimit() + step;
			if (newSize >= maxCache)
			{ // increase it until you overflow :D
				newSize = maxCache;
				qDebug() << m_consecutive_fast_evicitons
						 << tr("pixmap cache misses by eviction happened too fast, doing nothing as the cache size "
							   "reached it's limit");
			}
			else
			{
				qDebug() << m_consecutive_fast_evicitons
						 << tr("pixmap cache misses by eviction happened too fast, increasing cache size to")
						 << static_cast<int>(newSize);
			}
			_setCacheLimit(static_cast<int>(newSize));
			m_consecutive_fast_evicitons = 0;
			return true;
		}
		return false;
	}

	bool _setFastEvictionThreshold(int threshold)
	{
		m_consecutive_fast_evicitons_threshold = threshold;
		return true;
	}

  private:
	static PixmapCache* s_instance;
	QTime m_last_cache_miss_by_eviciton;
	int m_consecutive_fast_evicitons		   = 0;
	int m_consecutive_fast_evicitons_threshold = 15;
};

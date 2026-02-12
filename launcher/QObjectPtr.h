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

#include <QObject>
#include <QSharedPointer>

#include <functional>
#include <memory>

/**
 * A unique pointer class with unique pointer semantics intended for derivates of QObject
 * Calls deleteLater() instead of destroying the contained object immediately
 */
template <typename T>
using unique_qobject_ptr = QScopedPointer<T, QScopedPointerDeleteLater>;

/**
 * A shared pointer class with shared pointer semantics intended for derivates of QObject
 * Calls deleteLater() instead of destroying the contained object immediately
 */
template <typename T>
class shared_qobject_ptr : public QSharedPointer<T>
{
  public:
	constexpr explicit shared_qobject_ptr() : QSharedPointer<T>()
	{}
	constexpr explicit shared_qobject_ptr(T* ptr) : QSharedPointer<T>(ptr, &QObject::deleteLater)
	{}
	constexpr shared_qobject_ptr(std::nullptr_t null_ptr) : QSharedPointer<T>(null_ptr, &QObject::deleteLater)
	{}

	template <typename Derived>
	constexpr shared_qobject_ptr(const shared_qobject_ptr<Derived>& other) : QSharedPointer<T>(other)
	{}

	template <typename Derived>
	constexpr shared_qobject_ptr(const QSharedPointer<Derived>& other) : QSharedPointer<T>(other)
	{}

	void reset()
	{
		QSharedPointer<T>::reset();
	}
	void reset(T* other)
	{
		shared_qobject_ptr<T> t(other);
		this->swap(t);
	}
	void reset(const shared_qobject_ptr<T>& other)
	{
		shared_qobject_ptr<T> t(other);
		this->swap(t);
	}
};

template <typename T, typename... Args>
shared_qobject_ptr<T> makeShared(Args... args)
{
	auto obj = new T(args...);
	return shared_qobject_ptr<T>(obj);
}

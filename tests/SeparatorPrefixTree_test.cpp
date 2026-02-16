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

#include <QByteArray>
#include <QRandomGenerator>
#include <QSet>
#include <QStringList>
#include <QTest>

#include <SeparatorPrefixTree.h>

namespace {
QString randomPath(QRandomGenerator& rng)
{
	static const char kAlphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789-_.";
	const int alphabet_len = static_cast<int>(sizeof(kAlphabet) - 1);

	const int segments = rng.bounded(1, 5);
	QStringList parts;
	parts.reserve(segments);

	for (int i = 0; i < segments; ++i) {
		const int length = rng.bounded(1, 9);
		QByteArray bytes;
		bytes.reserve(length);
		for (int j = 0; j < length; ++j) {
			bytes.append(kAlphabet[rng.bounded(alphabet_len)]);
		}
		parts.append(QString::fromLatin1(bytes));
	}

	return parts.join('/');
}

QString shortestContainedPrefix(const SeparatorPrefixTree<'/'> &tree, const QString& path)
{
	const auto parts = path.split('/');
	QString prefix;
	for (int i = 0; i < parts.size(); ++i) {
		if (i == 0)
			prefix = parts.at(i);
		else
			prefix = prefix + '/' + parts.at(i);
		if (tree.contains(prefix))
			return prefix;
	}
	return QString();
}
} // namespace

class SeparatorPrefixTreeTest : public QObject
{
	Q_OBJECT

  private slots:
	void test_basicInsertAndCovers()
	{
		SeparatorPrefixTree<'/'> tree;
		tree.insert("a/b");
		tree.insert("a/c");
		tree.insert("d");

		QVERIFY(tree.contains("a/b"));
		QVERIFY(tree.contains("a/c"));
		QVERIFY(tree.contains("d"));
		QVERIFY(!tree.contains("a"));

		QVERIFY(tree.covers("a/b/c"));
		QCOMPARE(tree.cover("a/b/c"), QString("a/b"));
		QVERIFY(tree.covers("d/x"));
		QCOMPARE(tree.cover("d/x"), QString("d"));
		QVERIFY(!tree.covers("x"));
		QVERIFY(tree.cover("x").isNull());
	}

	void test_removeKeepsDescendants()
	{
		SeparatorPrefixTree<'/'> tree;
		tree.insert("a/b");
		tree.insert("a/b/c");
		tree.insert("x/y");

		QVERIFY(tree.remove("a/b"));
		QVERIFY(!tree.contains("a/b"));
		QVERIFY(tree.contains("a/b/c"));
		QVERIFY(tree.covers("a/b/c"));
		QCOMPARE(tree.cover("a/b/c"), QString("a/b/c"));
		QVERIFY(tree.contains("x/y"));
	}

	void test_toStringListMatchesContainment()
	{
		SeparatorPrefixTree<'/'> tree;
		QSet<QString> expected{ "alpha/bravo", "alpha/charlie", "delta" };
		for (const auto& path : expected)
			tree.insert(path);

		const auto list = tree.toStringList();
		QSet<QString> actual(list.cbegin(), list.cend());
		QCOMPARE(actual, expected);

		for (const auto& path : list)
			QVERIFY(tree.contains(path));
	}

	void test_randomizedStability()
	{
		SeparatorPrefixTree<'/'> tree;
		QSet<QString> inserted;
		QRandomGenerator rng(0xC0FFEEu);

		for (int i = 0; i < 250; ++i) {
			const auto path = randomPath(rng);
			inserted.insert(path);
			tree.insert(path);
			QVERIFY(tree.contains(path));
			QVERIFY(tree.covers(path));
			QCOMPARE(tree.cover(path), shortestContainedPrefix(tree, path));
		}

		for (int i = 0; i < 200; ++i) {
			const auto probe = randomPath(rng);
			const auto cover = tree.cover(probe);
			const auto expected = shortestContainedPrefix(tree, probe);
			if (expected.isNull()) {
				QVERIFY(!tree.covers(probe));
				QVERIFY(cover.isNull());
			} else {
				QVERIFY(tree.covers(probe));
				QCOMPARE(cover, expected);
			}
		}

		QStringList list = tree.toStringList();
		for (const auto& path : list)
			QVERIFY(tree.contains(path));

		const auto values = inserted.values();
		for (int i = 0; i < 80 && !inserted.isEmpty(); ++i) {
			const auto& path = values.at(rng.bounded(values.size()));
			if (tree.remove(path))
				inserted.remove(path);
			QVERIFY(!tree.contains(path));
		}
	}
};

QTEST_GUILESS_MAIN(SeparatorPrefixTreeTest)

#include "SeparatorPrefixTree_test.moc"

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

#include "FileLink.hpp"

#include "BuildConfig.h"
#include "StringUtils.h"

#include <QCommandLineParser>
#include <QDebug>
#include <QVariant>

#if defined Q_OS_WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include "console/WindowsConsole.hpp"
#endif

#include <filesystem>
#include <system_error>

namespace fs = std::filesystem;

namespace projt::filelink
{
	FileLinkApp::FileLinkApp(int& argc, char** argv) : QCoreApplication(argc, argv), m_socket(this)
	{
#if defined Q_OS_WIN32
		if (projt::console::AttachWindowsConsole())
		{
			m_consoleAttached = true;
		}
#endif
		setOrganizationName(BuildConfig.LAUNCHER_NAME);
		setOrganizationDomain(BuildConfig.LAUNCHER_DOMAIN);
		setApplicationName(BuildConfig.LAUNCHER_NAME + "FileLink");
		setApplicationVersion(BuildConfig.printableVersionString() + "\n" + BuildConfig.GIT_COMMIT);

		QCommandLineParser parser;
		parser.setApplicationDescription(QObject::tr("ProjT Launcher file link helper."));
		parser.addOptions({ { { "s", "server" }, "Join the specified server on launch", "pipe name" },
							{ { "H", "hard" }, "Use hard links instead of symlinks", "true/false" } });
		parser.addHelpOption();
		parser.addVersionOption();
		parser.process(arguments());

		const QString serverToJoin = parser.value("server");
		m_useHardLinks			   = QVariant(parser.value("hard")).toBool();

		qDebug() << "file link helper launched";
		m_input.setDevice(&m_socket);

		if (!serverToJoin.isEmpty())
		{
			qDebug() << "joining server" << serverToJoin;
			joinServer(serverToJoin);
			m_status = Status::Initialized;
		}
		else
		{
			qDebug() << "no server to join";
			m_status = Status::Failed;
		}
	}

	FileLinkApp::~FileLinkApp()
	{
		qDebug() << "file link helper shutting down";
		qInstallMessageHandler(nullptr);

#if defined Q_OS_WIN32
		if (m_consoleAttached)
		{
			fclose(stdout);
			fclose(stdin);
			fclose(stderr);
		}
#endif
	}

	void FileLinkApp::joinServer(const QString& serverName)
	{
		m_blockSize = 0;

		connect(&m_socket, &QLocalSocket::connected, this, []() { qDebug() << "connected to server"; });

		connect(&m_socket, &QLocalSocket::readyRead, this, &FileLinkApp::readPathPairs);

		connect(&m_socket,
				&QLocalSocket::errorOccurred,
				this,
				[this](QLocalSocket::LocalSocketError socketError)
				{
					m_status = Status::Failed;
					switch (socketError)
					{
						case QLocalSocket::ServerNotFoundError:
							qDebug() << ("The host was not found. Please make sure "
										 "that the server is running and that the "
										 "server name is correct.");
							break;
						case QLocalSocket::ConnectionRefusedError:
							qDebug() << ("The connection was refused by the peer. "
										 "Make sure the server is running, "
										 "and check that the server name "
										 "is correct.");
							break;
						case QLocalSocket::PeerClosedError:
							qDebug() << ("The connection was closed by the peer. ");
							break;
						default: qDebug() << "The following error occurred: " << m_socket.errorString();
					}
				});

		connect(&m_socket,
				&QLocalSocket::disconnected,
				this,
				[this]()
				{
					qDebug() << "disconnected from server, should exit";
					m_status = Status::Succeeded;
					exit();
				});

		m_socket.connectToServer(serverName);
	}

	void FileLinkApp::runLink()
	{
		qDebug() << "creating links";

		for (const auto& link : m_linksToMake)
		{
			const QString srcPath = link.src;
			const QString dstPath = link.dst;

			FS::ensureFilePathExists(dstPath);

			std::error_code osErr;
			if (m_useHardLinks)
			{
				qDebug() << "making hard link:" << srcPath << "to" << dstPath;
				fs::create_hard_link(StringUtils::toStdString(srcPath), StringUtils::toStdString(dstPath), osErr);
			}
			else if (fs::is_directory(StringUtils::toStdString(srcPath)))
			{
				qDebug() << "making directory_symlink:" << srcPath << "to" << dstPath;
				fs::create_directory_symlink(StringUtils::toStdString(srcPath),
											 StringUtils::toStdString(dstPath),
											 osErr);
			}
			else
			{
				qDebug() << "making symlink:" << srcPath << "to" << dstPath;
				fs::create_symlink(StringUtils::toStdString(srcPath), StringUtils::toStdString(dstPath), osErr);
			}

			if (osErr)
			{
				qWarning() << "Failed to link files:" << QString::fromStdString(osErr.message());
				qDebug() << "Source file:" << srcPath;
				qDebug() << "Destination file:" << dstPath;
				qDebug() << "Error category:" << osErr.category().name();
				qDebug() << "Error code:" << osErr.value();

				FS::LinkResult result = { srcPath, dstPath, QString::fromStdString(osErr.message()), osErr.value() };
				m_linkResults.append(result);
			}
			else
			{
				FS::LinkResult result = { srcPath, dstPath, "", 0 };
				m_linkResults.append(result);
			}
		}

		sendResults();
		qDebug() << "done, should exit soon";
	}

	void FileLinkApp::sendResults()
	{
		QByteArray block;
		QDataStream out(&block, QIODevice::WriteOnly);

		quint32 payloadSize = static_cast<quint32>(sizeof(quint32));
		for (const auto& result : m_linkResults)
		{
			payloadSize += static_cast<quint32>(result.src.size());
			payloadSize += static_cast<quint32>(result.dst.size());
			payloadSize += static_cast<quint32>(result.err_msg.size());
			payloadSize += static_cast<quint32>(sizeof(quint32));
		}
		qDebug() << "About to write block of size:" << payloadSize;
		out << payloadSize;

		out << static_cast<quint32>(m_linkResults.length());
		for (const auto& result : m_linkResults)
		{
			out << result.src;
			out << result.dst;
			out << result.err_msg;
			out << static_cast<quint32>(result.err_value);
		}

		qint64 bytesWritten = m_socket.write(block);
		bool bytesFlushed	= m_socket.flush();
		qDebug() << "block flushed" << bytesWritten << bytesFlushed;
	}

	void FileLinkApp::readPathPairs()
	{
		m_linksToMake.clear();
		qDebug() << "Reading path pairs from server";
		qDebug() << "bytes available" << m_socket.bytesAvailable();

		if (m_blockSize == 0)
		{
			if (m_socket.bytesAvailable() < static_cast<qint64>(sizeof(quint32)))
			{
				return;
			}
			qDebug() << "reading block size";
			m_input >> m_blockSize;
		}

		qDebug() << "blocksize is" << m_blockSize;
		qDebug() << "bytes available" << m_socket.bytesAvailable();
		if (m_socket.bytesAvailable() < static_cast<qint64>(m_blockSize) || m_input.atEnd())
		{
			return;
		}

		quint32 numLinks = 0;
		m_input >> numLinks;
		qDebug() << "numLinks" << numLinks;

		for (quint32 i = 0; i < numLinks; ++i)
		{
			FS::LinkPair pair;
			m_input >> pair.src;
			m_input >> pair.dst;
			qDebug() << "link" << pair.src << "to" << pair.dst;
			m_linksToMake.append(pair);
		}

		runLink();
	}
} // namespace projt::filelink

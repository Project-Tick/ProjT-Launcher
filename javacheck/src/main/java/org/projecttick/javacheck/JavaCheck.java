// SPDX-License-Identifier: GPL-2.0-only
// SPDX-FileCopyrightText: 2026 Project Tick
// SPDX-FileContributor: Project Tick Team
/*
 *
 *  Project Tick JavaCheck - A simple Java system property checker
 *  Copyright (C) 2026 Project Tick
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; version 2
 *  of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  License Version Restriction
 * 
 *  Redistribution, modification, or contribution to this program,
 *  including substantial or material contributions, does not grant
 *  the right to relicense the program, in whole or in part, under any
 *  later version of the GNU General Public License.
 *
 *  Any modified or derivative version of this program must be
 *  distributed under GPL version 2 only, unless an explicit written
 *  exception is granted by the original copyright holder.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, see
 *  <https://www.gnu.org/licenses/>.
 *
 */

package org.projecttick.javacheck;

public final class JavaCheck {

	public static void main(final String[] args) {
		if (args.length == 0) {
			System.err.println("Usage: <properties...>");
			System.exit(1);
		}

		boolean missingSome = false;

		for (final String property : args) {
			final String propertyValue = System.getProperty(property);

			if (propertyValue == null) {
				missingSome = true;
				continue;
			}

			System.out.println(property + '=' + propertyValue);
		}

		if (missingSome)
			System.exit(1);
	}

}
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
 *  along with this program; If not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

package org.projecttick.javacheck;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.TimeUnit;

public final class JavaCheck {

	public static void main(final String[] args) {
		if (args.length == 0) {
			System.err.println("Usage: <properties...> | --list [properties...]");
			System.exit(1);
		}

		final List<String> argList = new ArrayList<>(Arrays.asList(args));
		final boolean listAll = argList.remove("--list") || argList.remove("--scan");

		if (listAll) {
			final List<String> props = argList.isEmpty() ? defaultProperties() : argList;
			final List<Map<String, String>> runtimes = listJavaRuntimes(props);
			if (runtimes.isEmpty()) {
				System.exit(1);
			}
			for (Map<String, String> runtime : runtimes) {
				for (Map.Entry<String, String> entry : runtime.entrySet()) {
					System.out.println(entry.getKey() + '=' + entry.getValue());
				}
				System.out.println();
			}
			return;
		}

		boolean missingSome = false;

		for (final String property : argList) {
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

	private static List<String> defaultProperties() {
		return Arrays.asList(
			"java.version",
			"java.runtime.version",
			"java.vendor",
			"java.vendor.version",
			"java.vm.name",
			"java.vm.version",
			"java.home",
			"os.arch",
			"os.name",
			"os.version",
			"sun.arch.data.model"
		);
	}

	private static List<Map<String, String>> listJavaRuntimes(final List<String> props) {
		final List<Map<String, String>> results = new ArrayList<>();
		for (final Path javaPath : findJavaCandidates()) {
			final Map<String, String> propsMap = probeJava(javaPath, props);
			if (propsMap.isEmpty()) {
				continue;
			}
			final Map<String, String> output = new LinkedHashMap<>();
			output.put("java.path", javaPath.toString());
			for (final String key : props) {
				if (propsMap.containsKey(key)) {
					output.put(key, propsMap.get(key));
				}
			}
			results.add(output);
		}
		return results;
	}

	private static Set<Path> findJavaCandidates() {
		final Set<Path> candidates = new LinkedHashSet<>();
		final boolean isWindows = isWindows();
		final String javaExe = isWindows ? "java.exe" : "java";

		addCurrentRuntime(candidates, javaExe);
		addFromEnv(candidates, "JAVA_HOME", javaExe);
		addFromEnv(candidates, "JDK_HOME", javaExe);
		addFromEnv(candidates, "JRE_HOME", javaExe);

		addFromPathEnv(candidates, javaExe);

		final String sdkmanDir = System.getenv("SDKMAN_CANDIDATES_DIR");
		if (sdkmanDir != null && !sdkmanDir.isEmpty()) {
			addJavaFromParent(candidates, Paths.get(sdkmanDir, "java"), "bin", javaExe);
		}

		if (isWindows) {
			final String programFiles = System.getenv("ProgramFiles");
			final String programFilesX86 = System.getenv("ProgramFiles(x86)");
			final String programFilesW6432 = System.getenv("ProgramW6432");
			addWindowsRoots(candidates, programFiles, javaExe);
			addWindowsRoots(candidates, programFilesX86, javaExe);
			addWindowsRoots(candidates, programFilesW6432, javaExe);
		} else if (isMac()) {
			final Path root = Paths.get("/Library/Java/JavaVirtualMachines");
			addJavaFromParent(candidates, root, "Contents/Home/bin", javaExe);
			addJavaFromParent(candidates, root, "Contents/Home/jre/bin", javaExe);
		} else {
			addJavaFromParent(candidates, Paths.get("/usr/lib/jvm"), "bin", javaExe);
			addJavaFromParent(candidates, Paths.get("/usr/java"), "bin", javaExe);
			addJavaFromParent(candidates, Paths.get("/opt/java"), "bin", javaExe);
			addJavaFromParent(candidates, Paths.get("/opt/jdk"), "bin", javaExe);
			addJavaFromParent(candidates, Paths.get("/usr/local/java"), "bin", javaExe);
		}

		return candidates;
	}

	private static void addCurrentRuntime(final Set<Path> candidates, final String javaExe) {
		final String javaHome = System.getProperty("java.home");
		if (javaHome == null || javaHome.isEmpty()) {
			return;
		}
		addCandidate(candidates, Paths.get(javaHome, "bin", javaExe));
		final Path parent = Paths.get(javaHome).getParent();
		if (parent != null) {
			addCandidate(candidates, parent.resolve("bin").resolve(javaExe));
		}
	}

	private static void addFromEnv(final Set<Path> candidates, final String varName, final String javaExe) {
		final String envValue = System.getenv(varName);
		if (envValue == null || envValue.isEmpty()) {
			return;
		}
		addCandidate(candidates, Paths.get(envValue, "bin", javaExe));
		addCandidate(candidates, Paths.get(envValue, "jre", "bin", javaExe));
	}

	private static void addFromPathEnv(final Set<Path> candidates, final String javaExe) {
		final String pathEnv = System.getenv("PATH");
		if (pathEnv == null || pathEnv.isEmpty()) {
			return;
		}
		final String[] entries = pathEnv.split(File.pathSeparator);
		for (final String entry : entries) {
			if (entry == null || entry.isEmpty()) {
				continue;
			}
			addCandidate(candidates, Paths.get(entry, javaExe));
		}
	}

	private static void addWindowsRoots(final Set<Path> candidates, final String root, final String javaExe) {
		if (root == null || root.isEmpty()) {
			return;
		}
		addJavaFromParent(candidates, Paths.get(root, "Java"), "bin", javaExe);
		addJavaFromParent(candidates, Paths.get(root, "Eclipse Adoptium"), "bin", javaExe);
		addJavaFromParent(candidates, Paths.get(root, "AdoptOpenJDK"), "bin", javaExe);
		addJavaFromParent(candidates, Paths.get(root, "Amazon Corretto"), "bin", javaExe);
		addJavaFromParent(candidates, Paths.get(root, "Zulu"), "bin", javaExe);
	}

	private static void addJavaFromParent(final Set<Path> candidates, final Path parent, final String suffix, final String javaExe) {
		if (parent == null || !Files.isDirectory(parent)) {
			return;
		}
		try {
			Files.list(parent).filter(Files::isDirectory).forEach(dir -> addCandidate(candidates, dir.resolve(suffix).resolve(javaExe)));
		} catch (IOException ignored) {
		}
	}

	private static void addCandidate(final Set<Path> candidates, final Path path) {
		if (path == null) {
			return;
		}
		try {
			if (Files.isRegularFile(path) && Files.isExecutable(path)) {
				candidates.add(path.toRealPath());
			}
		} catch (IOException ignored) {
		}
	}

	private static Map<String, String> probeJava(final Path javaPath, final List<String> props) {
		final Map<String, String> values = new LinkedHashMap<>();
		final List<String> command = Arrays.asList(
			javaPath.toString(),
			"-XshowSettings:properties",
			"-version"
		);

		try {
			ProcessBuilder builder = new ProcessBuilder(command);
			builder.redirectErrorStream(true);
			Process process = builder.start();
			boolean finished = process.waitFor(10, TimeUnit.SECONDS);
			if (!finished) {
				process.destroyForcibly();
				return Collections.emptyMap();
			}

			final String output = readAll(process);
			final List<String> lines = Arrays.asList(output.split("\\R"));
			for (final String rawLine : lines) {
				final String line = rawLine.trim();
				final int idx = line.indexOf('=');
				if (idx <= 0) {
					continue;
				}
				final String key = line.substring(0, idx).trim();
				final String value = line.substring(idx + 1).trim();
				if (props.contains(key)) {
					values.put(key, value);
				}
			}
		} catch (Exception ignored) {
			return Collections.emptyMap();
		}

		return values;
	}

	private static String readAll(final Process process) throws IOException {
		final StringBuilder sb = new StringBuilder();
		try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
			String line;
			while ((line = reader.readLine()) != null) {
				sb.append(line).append('\n');
			}
		}
		return sb.toString();
	}

	private static boolean isWindows() {
		return osName().contains("win");
	}

	private static boolean isMac() {
		return osName().contains("mac");
	}

	private static String osName() {
		final String name = System.getProperty("os.name");
		return name == null ? "" : name.toLowerCase(Locale.ROOT);
	}
}

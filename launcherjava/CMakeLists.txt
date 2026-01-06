cmake_minimum_required(VERSION 3.15)
project(launcher Java)
find_package(Java 1.8 REQUIRED COMPONENTS Development)

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT tr.org.projecttick.projtlauncher.normal.EntryPoint)
set(CMAKE_JAVA_COMPILE_FLAGS -target 8 -source 8)

set(SRC
    tr/org/projecttick/projtlauncher/normal/EntryPoint.java
    tr/org/projecttick/projtlauncher/normal/launcher/Launcher.java
    tr/org/projecttick/projtlauncher/normal/launcher/impl/AbstractLauncher.java
    tr/org/projecttick/projtlauncher/normal/launcher/impl/StandardLauncher.java
    tr/org/projecttick/projtlauncher/normal/exception/ParameterNotFoundException.java
    tr/org/projecttick/projtlauncher/normal/exception/ParseException.java
    tr/org/projecttick/projtlauncher/normal/utils/Parameters.java
    tr/org/projecttick/projtlauncher/normal/utils/ReflectionUtils.java
    tr/org/projecttick/projtlauncher/normal/utils/logging/Level.java
    tr/org/projecttick/projtlauncher/normal/utils/logging/Log.java
    tr/org/projecttick/projtlauncher/normal/legacy/LegacyProxy.java
)

set(LEGACY_SRC
    tr/org/projecttick/projtlauncher/legacy/LegacyFrame.java
    tr/org/projecttick/projtlauncher/legacy/LegacyLauncher.java
    tr/org/projecttick/projtlauncher/legacy/fix/online/Handler.java
    tr/org/projecttick/projtlauncher/legacy/fix/online/OnlineFixes.java
    tr/org/projecttick/projtlauncher/legacy/fix/online/OnlineModeFix.java
    tr/org/projecttick/projtlauncher/legacy/fix/online/SkinFix.java
    tr/org/projecttick/projtlauncher/legacy/utils/Base64.java
    tr/org/projecttick/projtlauncher/legacy/utils/api/MojangApi.java
    tr/org/projecttick/projtlauncher/legacy/utils/api/Texture.java
    tr/org/projecttick/projtlauncher/legacy/utils/json/JsonParseException.java
    tr/org/projecttick/projtlauncher/legacy/utils/json/JsonParser.java
    tr/org/projecttick/projtlauncher/legacy/utils/url/ByteArrayUrlConnection.java
    tr/org/projecttick/projtlauncher/legacy/utils/url/UrlUtils.java
    net/minecraft/Launcher.java
    legacyshim/LegacyProxy.java
)

add_jar(ProjTLaunch ${SRC})
add_jar(ProjTLaunchLegacy ${LEGACY_SRC} INCLUDE_JARS ProjTLaunch)
install_jar(ProjTLaunch "${JARS_DEST_DIR}")
install_jar(ProjTLaunchLegacy "${JARS_DEST_DIR}")

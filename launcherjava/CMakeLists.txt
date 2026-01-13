cmake_minimum_required(VERSION 3.15)
project(launcher Java)
find_package(Java 1.8 REQUIRED COMPONENTS Development)

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT org.projecttick.projtlauncher.normal.EntryPoint)
set(CMAKE_JAVA_COMPILE_FLAGS -target 8 -source 8)

set(SRC
    org/projecttick/projtlauncher/normal/EntryPoint.java
    org/projecttick/projtlauncher/normal/launcher/Launcher.java
    org/projecttick/projtlauncher/normal/launcher/impl/AbstractLauncher.java
    org/projecttick/projtlauncher/normal/launcher/impl/StandardLauncher.java
    org/projecttick/projtlauncher/normal/exception/ParameterNotFoundException.java
    org/projecttick/projtlauncher/normal/exception/ParseException.java
    org/projecttick/projtlauncher/normal/utils/Parameters.java
    org/projecttick/projtlauncher/normal/utils/ReflectionUtils.java
    org/projecttick/projtlauncher/normal/utils/logging/Level.java
    org/projecttick/projtlauncher/normal/utils/logging/Log.java
    org/projecttick/projtlauncher/normal/legacy/LegacyProxy.java
)

set(LEGACY_SRC
    org/projecttick/projtlauncher/legacy/LegacyFrame.java
    org/projecttick/projtlauncher/legacy/LegacyLauncher.java
    org/projecttick/projtlauncher/legacy/fix/online/Handler.java
    org/projecttick/projtlauncher/legacy/fix/online/OnlineFixes.java
    org/projecttick/projtlauncher/legacy/fix/online/OnlineModeFix.java
    org/projecttick/projtlauncher/legacy/fix/online/SkinFix.java
    org/projecttick/projtlauncher/legacy/utils/Base64.java
    org/projecttick/projtlauncher/legacy/utils/api/MojangApi.java
    org/projecttick/projtlauncher/legacy/utils/api/Texture.java
    org/projecttick/projtlauncher/legacy/utils/json/JsonParseException.java
    org/projecttick/projtlauncher/legacy/utils/json/JsonParser.java
    org/projecttick/projtlauncher/legacy/utils/url/ByteArrayUrlConnection.java
    org/projecttick/projtlauncher/legacy/utils/url/UrlUtils.java
    net/minecraft/Launcher.java
    legacyshim/LegacyProxy.java
)

add_jar(ProjTLaunch ${SRC})
add_jar(ProjTLaunchLegacy ${LEGACY_SRC} INCLUDE_JARS ProjTLaunch)
install_jar(ProjTLaunch "${JARS_DEST_DIR}")
install_jar(ProjTLaunchLegacy "${JARS_DEST_DIR}")

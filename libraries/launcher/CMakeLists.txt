cmake_minimum_required(VERSION 3.15)
project(launcher Java)
find_package(Java 1.7 REQUIRED COMPONENTS Development)
set(JARS_DEST_DIR "${CMAKE_INSTALL_PREFIX}/jars")

include(UseJava)
set(CMAKE_JAVA_JAR_ENTRY_POINT tr.org.yongdohyun.projtlauncher.EntryPoint)
set(CMAKE_JAVA_COMPILE_FLAGS -target 7 -source 7)

set(SRC
    tr/org/yongdohyun/projtlauncher/normal/EntryPoint.java
    tr/org/yongdohyun/projtlauncher/normal/launcher/Launcher.java
    tr/org/yongdohyun/projtlauncher/normal/launcher/impl/AbstractLauncher.java
    tr/org/yongdohyun/projtlauncher/normal/launcher/impl/StandardLauncher.java
    tr/org/yongdohyun/projtlauncher/normal/exception/ParameterNotFoundException.java
    tr/org/yongdohyun/projtlauncher/normal/exception/ParseException.java
    tr/org/yongdohyun/projtlauncher/normal/utils/Parameters.java
    tr/org/yongdohyun/projtlauncher/normal/utils/ReflectionUtils.java
    tr/org/yongdohyun/projtlauncher/normal/utils/logging/Level.java
    tr/org/yongdohyun/projtlauncher/normal/utils/logging/Log.java
    tr/org/yongdohyun/projtlauncher/normal/legacy/LegacyProxy.java
)

set(LEGACY_SRC
    tr/org/yongdohyun/projtlauncher/legacy/LegacyFrame.java
    tr/org/yongdohyun/projtlauncher/legacy/LegacyLauncher.java
    tr/org/yongdohyun/projtlauncher/legacy/fix/online/Handler.java
    tr/org/yongdohyun/projtlauncher/legacy/fix/online/OnlineFixes.java
    tr/org/yongdohyun/projtlauncher/legacy/fix/online/OnlineModeFix.java
    tr/org/yongdohyun/projtlauncher/legacy/fix/online/SkinFix.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/Base64.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/api/MojangApi.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/api/Texture.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/json/JsonParseException.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/json/JsonParser.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/url/ByteArrayUrlConnection.java
    tr/org/yongdohyun/projtlauncher/legacy/utils/url/UrlUtils.java
    net/minecraft/Launcher.java
    tr/org/yongdohyun/projtlauncher/legacy/LegacyProxy.java
)

add_jar(ProjTLaunch ${SRC})
add_jar(ProjTLaunchLegacy ${LEGACY_SRC} INCLUDE_JARS ProjTLaunch)
install_jar(ProjTLaunch "${JARS_DEST_DIR}")
install_jar(ProjTLaunchLegacy "${JARS_DEST_DIR}")

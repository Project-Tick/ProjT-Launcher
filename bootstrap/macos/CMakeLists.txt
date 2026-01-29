find_package(LibXml2 REQUIRED)

set(BOOTSTRAP_NAME "ProjT Launcher Installer")
set(BOOTSTRAP_ICNS "${CMAKE_SOURCE_DIR}/${Launcher_Branding_ICNS}")
get_filename_component(BOOTSTRAP_ICNS_NAME "${BOOTSTRAP_ICNS}" NAME)
set(BOOTSTRAP_BUNDLE_ID "org.projecttick.ProjTLauncherInstaller")

add_executable(ProjTLauncherBootstrap MACOSX_BUNDLE
    Bootstrap.m
)

target_include_directories(ProjTLauncherBootstrap PRIVATE ${LIBXML2_INCLUDE_DIR})
target_link_libraries(ProjTLauncherBootstrap PRIVATE ${LIBXML2_LIBRARIES})
target_link_libraries(ProjTLauncherBootstrap PRIVATE "-framework CoreFoundation")
target_link_libraries(ProjTLauncherBootstrap PRIVATE "-framework Cocoa")

target_compile_definitions(ProjTLauncherBootstrap PRIVATE
    BOOTSTRAP_FEED_URL="${MACOSX_BOOTSTRAP_FEED_URL}"
    BOOTSTRAP_URL_TEMPLATE="${MACOSX_BOOTSTRAP_URL_TEMPLATE}"
)

set_target_properties(ProjTLauncherBootstrap PROPERTIES
    OUTPUT_NAME "${BOOTSTRAP_NAME}"
    MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    MACOSX_BUNDLE_BUNDLE_NAME "${BOOTSTRAP_NAME}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "${BOOTSTRAP_BUNDLE_ID}"
    MACOSX_BUNDLE_BUNDLE_VERSION "${Launcher_VERSION_NAME}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${Launcher_VERSION_NAME}"
    MACOSX_BUNDLE_LONG_VERSION_STRING "${Launcher_VERSION_NAME}"
    MACOSX_BUNDLE_COPYRIGHT "${Launcher_Copyright_Mac}"
    MACOSX_BUNDLE_INFO_STRING "${MACOSX_BUNDLE_INFO_STRING}"
    MACOSX_BUNDLE_ICON_FILE "${BOOTSTRAP_ICNS_NAME}"
)

target_sources(ProjTLauncherBootstrap PRIVATE "${BOOTSTRAP_ICNS}")
set_source_files_properties("${BOOTSTRAP_ICNS}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

install(TARGETS ProjTLauncherBootstrap BUNDLE DESTINATION "." COMPONENT launcher)

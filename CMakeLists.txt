cmake_minimum_required(VERSION 3.3)

project(upnp-qt)

find_package(ECM 5.15.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION "5.4.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Xml Qml WebSockets)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG OPTIONAL_COMPONENTS Quick Widgets Multimedia Svg Test)

find_package(KDSoap CONFIG REQUIRED)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(CMakePackageConfigHelpers)

ecm_setup_version(0.1.0 VARIABLE_PREFIX UPNP_QT
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/upnpQt_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/UPNPQTConfigVersion.cmake")

if (CMAKE_SYSTEM_NAME STREQUAL Android)
    set(QT_QMAKE_EXECUTABLE "$ENV{Qt5_android}/bin/qmake")
    # workaround until this fix is in released ECM
    if(ECM_VERSION VERSION_LESS "5.15.0")
        add_definitions(-DANDROID)
    endif()
endif()

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(autotests)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/UPNPQT")
configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/UPNPQTConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/UPNPQTConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/UPNPQTConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/UPNPQTConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT UPNPQTTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE UPNPQTTargets.cmake NAMESPACE UPNP:: COMPONENT Devel)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)


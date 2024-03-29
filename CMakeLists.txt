# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)

project(UpnpLibQt VERSION 0.1)

set(KF5_VERSION "5.59.0")

include(FeatureSummary)
find_package(ECM ${KF5_VERSION}  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)
include(GenerateExportHeader)

set(REQUIRED_QT_VERSION 6.4.0)
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Xml)
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG OPTIONAL_COMPONENTS Qml Quick Widgets Test)

find_package(KDSoap-qt6 1.5.0 CONFIG REQUIRED)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)
include(ECMAddQch)

option(BUILD_QCH "Build API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)" OFF)
add_feature_info(QCH ${BUILD_QCH} "API documentation in QCH format (for e.g. Qt Assistant, Qt Creator & KDevelop)")

ecm_setup_version(PROJECT
    VARIABLE_PREFIX UPNPLIBQT
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/upnplibqt_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/UpnpLibQtConfigVersion.cmake"
    SOVERSION 5)
add_definitions(-DQT_NO_FOREACH)

add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/UpnpLibQt")

if (BUILD_QCH)
    ecm_install_qch_export(
        TARGETS UpnpLibQt_QCH
        FILE UpnpLibQtQchTargets.cmake
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
    )
    set(PACKAGE_INCLUDE_QCHTARGETS "include(\"\${CMAKE_CURRENT_LIST_DIR}/UpnpLibQtQchTargets.cmake\")")
endif()

include(CMakePackageConfigHelpers)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/UpnpLibQtConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/UpnpLibQtConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/upnplibqt_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR}
        COMPONENT Devel)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/UpnpLibQtConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/UpnpLibQtConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT UpnpLibQtTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE UpnpLibQtTargets.cmake
        NAMESPACE UPNP::)

install(FILES upnplibqt.categories DESTINATION  ${KDE_INSTALL_LOGGINGCATEGORIESDIR})

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

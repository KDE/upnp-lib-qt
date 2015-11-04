cmake_minimum_required(VERSION 3.3)

project(upnp-player-qt)

find_package(ECM 5.15.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_SOURCE_DIR}/cmake/modules) #TODO: remove cmake/modules when extra-cmake-modules is done

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings)
include(KDECMakeSettings)

set(REQUIRED_QT_VERSION "5.4.0")
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Network Xml)
find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG OPTIONAL_COMPONENTS Quick Widgets Multimedia Svg Test)

find_package(KDSoap CONFIG REQUIRED)
find_package(KF5Declarative CONFIG)

include(FeatureSummary)
include(GenerateExportHeader)
include(ECMSetupVersion)
include(ECMGenerateHeaders)

ecm_setup_version(0.1.0 VARIABLE_PREFIX UPNP_QT
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/upnpQt_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/UpnpQtConfigVersion.cmake")

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

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)


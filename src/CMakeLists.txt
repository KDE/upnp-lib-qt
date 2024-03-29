# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

# SPDX-License-Identifier: BSD-2-Clause

set(upnpLibQt_SRCS
    upnpssdpengine.cpp
    upnpcontrolabstractservice.cpp
    upnpcontrolabstractservicereply.cpp
    upnpcontrolabstractdevice.cpp
    upnphttpserver.cpp
    upnpservereventobject.cpp
    upnpabstractdevice.cpp
    upnpabstractservice.cpp
    upnpdevicesoapserver.cpp
    upnpdevicesoapserverobject.cpp
    upnpbasictypes.h
    upnpeventsubscriber.cpp
    upnpdevicedescriptionparser.cpp
    upnpservicedescriptionparser.cpp
    upnpdiscoveryresult.cpp
    upnpdevicedescription.cpp
    upnpactiondescription.cpp
    upnpservicedescription.cpp
    upnpstatevariabledescription.cpp
)

ecm_qt_declare_logging_category(upnpLibQt_SRCS
    HEADER "ssdplogging.h"
    IDENTIFIER "orgKdeUpnpLibQtSsdp"
    CATEGORY_NAME "org.kde.upnplibqt.ssdp"
    DEFAULT_SEVERITY Info
    )

ecm_qt_declare_logging_category(upnpLibQt_SRCS
    HEADER "upnplogging.h"
    IDENTIFIER "orgKdeUpnpLibQtUpnp"
    CATEGORY_NAME "org.kde.upnplibqt.upnp"
    DEFAULT_SEVERITY Info
    )

add_library(UpnpLibQt ${upnpLibQt_SRCS})
generate_export_header(UpnpLibQt BASE_NAME UpnpLibQt)


target_include_directories(UpnpLibQt
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF}/UpnpLibQt>"
)

set_target_properties(UpnpLibQt PROPERTIES
    VERSION     0.1
    SOVERSION   0
    EXPORT_NAME "UpnpLibQt"
)

ecm_generate_headers(UpnpLibQt_HEADERS
    HEADER_NAMES
    UpnpBasicTypes
    UpnpAbstractDevice
    UpnpAbstractService
    UpnpControlAbstractService
    UpnpControlAbstractServiceReply
    UpnpControlAbstractDevice
    UpnpEventSubscriber
    UpnpSsdpEngine
    UpnpDiscoveryResult
    UpnpDeviceDescriptionParser
    UpnpHttpServer
    UpnpServerEventObject
    UpnpDeviceSoapServer
    UpnpDeviceSoapServerObject
    UpnpDeviceDescription
    UpnpActionDescription
    UpnpServiceDescription
    UpnpStateVariableDescription

    REQUIRED_HEADERS UpnpLibQt_HEADERS
)

install(TARGETS UpnpLibQt
        EXPORT UpnpLibQtTargets
        ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/upnplibqt_export.h
            ${UpnpLibQt_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF}/UpnpLibQt
        COMPONENT Devel)

if(BUILD_QCH)
    ecm_add_qch(
        UpnpLibQt_QCH
        NAME UpnpLibQt
        BASE_NAME UpnpLibQt
        VERSION ${KF5_VERSION}
        ORG_DOMAIN org.kde
        SOURCES # using only public headers, to cover only public API
            ${UpnpLibQt_HEADERS}
        MD_MAINPAGE "${CMAKE_SOURCE_DIR}/README.md"
        LINK_QCHS
            Qt6Core_QCH
        BLANK_MACROS
            UPNPLIBQT_EXPORT
        TAGFILE_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        QCH_INSTALL_DESTINATION ${KDE_INSTALL_QTQCHDIR}
        COMPONENT Devel
    )
endif()

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME UpnpLibQt LIB_NAME UpnpLibQt DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF}/UpnpLibQt)
install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

target_link_libraries(UpnpLibQt
    LINK_PUBLIC
        Qt::Core
        Qt::Network
    LINK_PRIVATE
        Qt::Xml
        KDSoap::kdsoap
        KDSoap::kdsoap-server
)

target_include_directories(UpnpLibQt PUBLIC ${KDSoap_INCLUDE_DIRS})

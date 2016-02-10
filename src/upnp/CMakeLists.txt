add_definitions(-std=c++11)

set(upnpQt_SOURCES
    upnpssdpengine.cpp
    upnpcontrolabstractservice.cpp
    upnpcontrolabstractdevice.cpp
    upnphttpserver.cpp
    upnpservereventobject.cpp
    upnpcontrolrenderingcontrol.cpp
    upnpcontrolavtransport.cpp
    upnpcontrolswitchpower.cpp
    upnpcontroldimming.cpp
    upnpcontrolconnectionmanager.cpp
    upnpdevicemodel.cpp
    upnpabstractdevice.cpp
    upnpabstractservice.cpp
    upnpdevicesoapserver.cpp
    upnpdevicesoapserverobject.cpp
    upnpbasictypes.h
    upnpeventsubscriber.cpp
    upnpcontrolmediaserver.cpp
    upnpcontrolcontentdirectory.cpp
    upnpcontentdirectorymodel.cpp
    upnpdevicedescriptionparser.cpp
    upnpservicedescriptionparser.cpp
    upnpdiscoveryresult.cpp
)

ecm_generate_headers(
  upnpQt_CamelCase_HEADERS
  HEADER_NAMES
    UpnpBasicTypes
    UpnpAbstractDevice
    UpnpAbstractService
    UpnpControlAbstractService
    UpnpControlAbstractDevice
    UpnpControlRenderingControl
    UpnpControlAVTransport
    UpnpControlSwitchpower
    UpnpControlDimming
    UpnpControlConnectionManager
    UpnpControlContentDirectory
    UpnpControlMediaServer
    UpnpDeviceModel
    UpnpContentDirectoryModel
    UpnpEventSubscriber
    UpnpSsdpEngine
    UpnpDiscoveryResult
    UpnpDeviceDescriptionParser
    UpnpHttpServer
    UpnpServerEventObject
    UpnpDeviceSoapServer
    UpnpDeviceSoapServerObject

  PREFIX upnpQt
  REQUIRED_HEADERS upnpQt_HEADERS
)

add_library(upnpQt SHARED ${upnpQt_SOURCES})

generate_export_header(upnpQt BASE_NAME upnpQt
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/upnpQt_export.h
)

target_link_libraries(upnpQt
    LINK_PRIVATE
        Qt5::Network
        Qt5::Xml
)

target_link_libraries(upnpQt
    LINK_PUBLIC
        Qt5::Core
        KDSoap::kdsoap
        KDSoap::kdsoap-server
        upnpQtBase
)

target_include_directories(upnpQt PUBLIC ${KDSoap_INCLUDE_DIRS})

target_include_directories(upnpQt PUBLIC "$<BUILD_INTERFACE:${upnpQt_INCLUDE_DIRS}>")

target_include_directories(upnpQt INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KF5/UPNPQT/upnpQt>")

target_include_directories(upnpQt PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>")

set_target_properties(upnpQt PROPERTIES VERSION ${UPNP_QT_VERSION_STRING}
                                               SOVERSION ${UPNP_QT_SOVERSION}
                                               LINK_FLAGS "-Wl,--no-undefined")


install(TARGETS upnpQt EXPORT UPNPQTTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS upnpQt ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${upnpQt_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/UPNPQT/UPNPQT COMPONENT Devel)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/upnpQt_export.h
  ${upnpQt_HEADERS}
  DESTINATION  ${KDE_INSTALL_INCLUDEDIR_KF5}/UPNPQT/upnpQt COMPONENT Devel
)

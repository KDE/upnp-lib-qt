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
    upnpbinarylight.cpp
    upnpswitchpower.cpp
    upnpbasictypes.h
    upnpeventsubscriber.cpp
    upnpcontrolmediaserver.cpp
    upnpcontrolcontentdirectory.cpp
    upnpcontentdirectorymodel.cpp
    mediaplaylist.cpp
    upnpdevicedescriptionparser.cpp
    upnpservicedescriptionparser.cpp
)

ecm_generate_headers(
  upnpQt_CamelCase_HEADERS
  HEADER_NAMES
    UpnpSsdpEngine
    UpnpControlAbstractService
    UpnpControlAbstractDevice
    UpnpHttpServer
    UpnpServerEventObject
    UpnpControlRenderingControl
    UpnpControlAVTransport
    UpnpControlSwitchpower
    UpnpControlDimming
    UpnpDeviceModel
    UpnpAbstractDevice
    UpnpAbstractService
    UpnpDeviceSoapServer
    UpnpDeviceSoapServerObject
    UpnpBinaryLight
    UpnpSwitchPower
    UpnpBasicTypes
    UpnpEventSubscriber

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
        KDSoap::kdsoap-server
)

target_link_libraries(upnpQt
    LINK_PUBLIC
        Qt5::Core
        KDSoap::kdsoap
        upnpQtBase
)

target_include_directories(upnpQt PRIVATE ${KDSoap_INCLUDE_DIRS})

target_include_directories(upnpQt PUBLIC "$<BUILD_INTERFACE:${upnpQt_INCLUDE_DIRS}>")

target_include_directories(upnpQt INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/upnpQt>")

target_include_directories(upnpQt PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>")

set_target_properties(upnpQt PROPERTIES VERSION ${UPNP_QT_VERSION_STRING}
                                               SOVERSION ${UPNP_QT_SOVERSION}
                                               LINK_FLAGS "-Wl,--no-undefined")


install(TARGETS upnpQt ${INSTALL_TARGETS_DEFAULT_ARGS})

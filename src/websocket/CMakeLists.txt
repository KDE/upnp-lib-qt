add_definitions(-std=c++11)

set(upnpQtWebSocket_SOURCES
    upnpproxywebsocket.cpp
    upnpwebsocketserversocket.cpp
    upnpwebsocketbaseclient.cpp
    upnpwebsocketclient.cpp
    upnpwebsocketpublisher.cpp
    upnpwebsocketinternalclient.cpp
    upnpwebsocketcertificateconfiguration.cpp
    upnpwebsocketabstractservicecontrol.cpp
    upnpwebsocketdevicenotifier.cpp
    upnpwebsocketcommon.cpp
    upnpwebsocketeventsubscriber.cpp
)

ecm_generate_headers(
  upnpQtWebSocket_CamelCase_HEADERS
  HEADER_NAMES
    UpnpProxyWebSocket
    UpnpWebSocketServerSocket

  PREFIX upnpQtWebSocket
  REQUIRED_HEADERS upnpQtWebSocket_HEADERS
)

add_library(upnpQtWebSocket SHARED ${upnpQtWebSocket_SOURCES})

generate_export_header(upnpQtWebSocket BASE_NAME upnpQtWebSocket
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/upnpQtWebSocket_export.h
)

target_link_libraries(upnpQtWebSocket
    LINK_PRIVATE
        Qt5::Network
        Qt5::Xml
        Qt5::WebSockets
)

target_link_libraries(upnpQtWebSocket
    LINK_PUBLIC
        Qt5::Core
        upnpQtBase
)

target_include_directories(upnpQtWebSocket PRIVATE ${KDSoap_INCLUDE_DIRS})

target_include_directories(upnpQtWebSocket PUBLIC "$<BUILD_INTERFACE:${upnpQtWebSocket_INCLUDE_DIRS}>")

target_include_directories(upnpQtWebSocket INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KF5/UPNPQTWEBSOCKET/upnpQtWebSocket>")

target_include_directories(upnpQtWebSocket PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>")

set_target_properties(upnpQtWebSocket PROPERTIES VERSION ${UPNP_QT_VERSION_STRING}
                                               SOVERSION ${UPNP_QT_SOVERSION}
                                               LINK_FLAGS "-Wl,--no-undefined")


install(TARGETS upnpQtWebSocket EXPORT UPNPQTTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS upnpQtWebSocket ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${upnpQtWebSocket_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/UPNPQTWEBSOCKET/UPNPQTWBSOCKET COMPONENT Devel)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/upnpQtWebSocket_export.h
  ${upnpQtWebSocket_HEADERS}
  DESTINATION  ${KDE_INSTALL_INCLUDEDIR_KF5}/UPNPQTWEBSOCKET/upnpQtWebSocket COMPONENT Devel
)

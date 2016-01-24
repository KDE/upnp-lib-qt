include_directories(${upnp-qt_BINARY_DIR})

set(upnpWebSocketProxy_SOURCES
    proxy.cpp
    proxy.qml
)

qt5_add_resources(upnpWebSocketProxy_SOURCES proxy.qrc)

add_executable(upnpWebSocketProxy ${upnpWebSocketProxy_SOURCES})

target_link_libraries(upnpWebSocketProxy
    LINK_PRIVATE
        upnpQtWebSocket
        Qt5::Qml
        Qt5::Core
)

set(upnpWebSocketClient_SOURCES
    client.cpp
    upnpwebsocketdevicemodel.cpp
    upnpwebsocketcontrolswitchpower.cpp
    client.qml
)

qt5_add_resources(upnpWebSocketClient_SOURCES client.qrc)

add_executable(upnpWebSocketClient ${upnpWebSocketClient_SOURCES})

target_link_libraries(upnpWebSocketClient
    LINK_PRIVATE
        upnpQtWebSocket
        Qt5::Quick
        Qt5::Widgets
)

set(binaryLight_SOURCES
    binaryLight.cpp
    binaryLight.qml
    upnpbinarylight.cpp
    upnpswitchpower.cpp
    abstractrelayactuator.cpp
    olinuxinoremoterelay.cpp
)

qt5_add_resources(binaryLight_SOURCES binaryLight.qrc)

add_executable(binaryLight ${binaryLight_SOURCES})

target_link_libraries(binaryLight
    LINK_PRIVATE
        upnpQtWebSocket
        upnpQt
        Qt5::Qml
        Qt5::Core
)

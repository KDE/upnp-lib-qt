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
        Qt5::Quick
        Qt5::Widgets
)

set(upnpWebSocketClient_SOURCES
    client.cpp
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


set(upnp-player-qt_SOURCES
    main.cpp
    upnpssdpengine.cpp
    upnpcontrolabstractservice.cpp
    upnpcontrolabstractdevice.cpp
    upnphttpserver.cpp
    upnpservereventobject.cpp
    upnpcontrolrenderingcontrol.cpp
    upnpcontrolavtransport.cpp
    upnpcontrolswitchpower.cpp
    upnpcontroldimming.cpp
    upnpdevicemodel.cpp
    upnpabstractdevice.cpp
    upnpabstractservice.cpp
    upnpdevicesoapserver.cpp
    upnpbasictypes.h
    main.qml
)

add_executable(upnp-player-qt ${upnp-player-qt_SOURCES})

target_include_directories(upnp-player-qt PRIVATE ${KDSoap_INCLUDE_DIRS})

target_link_libraries(upnp-player-qt
    LINK_PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Quick
        Qt5::Widgets
        Qt5::Xml
        KDSoap::kdsoap
        KDSoap::kdsoap-server
)

install(TARGETS upnp-player-qt ${INSTALL_TARGETS_DEFAULT_ARGS})


set(upnp-player-qt_SOURCES
    main.cpp
    upnplistenner.cpp
    upnpservicedescription.cpp
    upnpdevicedescription.cpp
    upnphttpserver.cpp
    main.qml
)

link_directories(${LIBUPNP_LIBRARY_DIRS})

add_executable(upnp-player-qt ${upnp-player-qt_SOURCES})

target_include_directories(upnp-player-qt PRIVATE ${LIBUPNP_INCLUDE_DIRS} ${KDSoap_INCLUDE_DIRS})

target_link_libraries(upnp-player-qt
    LINK_PRIVATE
        Qt5::Core
        Qt5::Network
        Qt5::Quick
        Qt5::Widgets
        Qt5::Xml
        KDSoap::kdsoap
        KDSoap::kdsoap-server
        ${LIBUPNP_LIBRARIES}
)

install(TARGETS upnp-player-qt ${INSTALL_TARGETS_DEFAULT_ARGS})


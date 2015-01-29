set(upnp-player-qt_SOURCES
    main.cpp
    upnplistenner.cpp
)

link_directories(${LIBUPNP_LIBRARY_DIRS})

add_executable(upnp-player-qt ${upnp-player-qt_SOURCES})

target_include_directories(upnp-player-qt PRIVATE ${LIBUPNP_INCLUDE_DIRS} ${KDSoap_INCLUDE_DIRS})

target_link_libraries(upnp-player-qt
    LINK_PRIVATE
        Qt5::Core
        Qt5::Network
        KDSoap::kdsoap
        ${LIBUPNP_LIBRARIES}
)

install(TARGETS upnp-player-qt ${INSTALL_TARGETS_DEFAULT_ARGS})


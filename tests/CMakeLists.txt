if (KF5Declarative_FOUND)
    set(upnpControl_SOURCES
        upnpControl.cpp
        upnpControl.qml
        mediaServer.qml
        mediaServerListing.qml
        mediaAlbumView.qml
        genericDevice.qml
        RatingStar.qml
        AudioTrackDelegate.qml
        MediaPlayerControl.qml
        MediaServerEntry.qml
        MediaContentDirectory.qml
        MediaPlayListView.qml
        PlayListControler.qml
    )

    qt5_add_resources(upnpControl_SOURCES upnpControl.qrc)

    add_executable(upnpControl ${upnpControl_SOURCES})

    target_include_directories(upnpControl PRIVATE ${KDSoap_INCLUDE_DIRS})

    target_link_libraries(upnpControl
        LINK_PRIVATE
            upnpQt
            Qt5::Quick
            Qt5::Widgets
            KF5::Declarative
    )
endif()

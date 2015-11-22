if (KF5Declarative_FOUND)
    set(upnpControl_SOURCES
        upnpControl.cpp
        mediaplaylist.cpp
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

set(upnpControlMobile_SOURCES
    upnpControlMobile.cpp
    mediaplaylist.cpp
    AudioTrackDelegateMobile.qml
    genericDeviceMobile.qml
    upnpControlMobile.qml
    mediaServerMobile.qml
    mediaServerListingMobile.qml
    mediaAlbumViewMobile.qml
    MediaPlayerControlMobile.qml
    MediaServerEntryMobile.qml
    MediaContentDirectoryMobile.qml
    PlayListControler.qml
)

qt5_add_resources(upnpControlMobile_SOURCES upnpControlMobile.qrc)

add_executable(upnpControlMobile ${upnpControlMobile_SOURCES})

target_include_directories(upnpControlMobile PRIVATE ${KDSoap_INCLUDE_DIRS})

target_link_libraries(upnpControlMobile
    LINK_PRIVATE
        upnpQt
        Qt5::Quick
        Qt5::Widgets
)

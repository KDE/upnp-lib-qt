include_directories(${upnp-qt_BINARY_DIR})

set(upnpControl_SOURCES
    upnpControl.cpp
    mediaplaylist.cpp
    playlistcontroler.cpp
    viewpagesmodel.cpp
    remoteserverentry.cpp
    upnpControl.qml
    MediaServer.qml
    genericDevice.qml
    MediaServerListing.qml
    MediaAlbumView.qml
    RatingStar.qml
    AudioTrackDelegate.qml
    MediaPlayerControl.qml
    MediaServerEntry.qml
    MediaContentDirectory.qml
    MediaPlayListView.qml
    AudioTrackDelegateMobile.qml
    genericDeviceMobile.qml
    upnpControlMobile.qml
    mediaServerMobile.qml
    mediaServerListingMobile.qml
    mediaAlbumViewMobile.qml
    MediaPlayerControlMobile.qml
    MediaServerEntryMobile.qml
    MediaContentDirectoryMobile.qml
    HeaderBar.qml
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

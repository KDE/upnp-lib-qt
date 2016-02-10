add_definitions(-std=c++11)

set(upnpQtBase_SOURCES
    upnpdevicedescription.cpp
    upnpservicedescription.cpp
    upnpactiondescription.cpp
    upnpstatevariabledescription.cpp
)

ecm_generate_headers(
  upnpQtBase_CamelCase_HEADERS
  HEADER_NAMES
    UpnpDeviceDescription
    UpnpServiceDescription
    UpnpActionDescription
    UpnpStateVariableDescription

  PREFIX upnpQtBase
  REQUIRED_HEADERS upnpQtBase_HEADERS
)

add_library(upnpQtBase SHARED ${upnpQtBase_SOURCES})

generate_export_header(upnpQtBase BASE_NAME upnpQtBase
  EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/upnpQtBase_export.h
)

# target_link_libraries(upnpQtBase
#     LINK_PRIVATE
# )

target_link_libraries(upnpQtBase
    LINK_PUBLIC
        Qt5::Core
)

target_include_directories(upnpQtBase PUBLIC "$<BUILD_INTERFACE:${upnpQtBase_INCLUDE_DIRS}>")

target_include_directories(upnpQtBase INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/KF5/UPNPQTBASE/upnpQtBase>")

target_include_directories(upnpQtBase PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/..>")

set_target_properties(upnpQtBase PROPERTIES VERSION ${UPNP_QT_VERSION_STRING}
                                               SOVERSION ${UPNP_QT_SOVERSION}
                                               LINK_FLAGS "-Wl,--no-undefined")


install(TARGETS upnpQtBase EXPORT UPNPQTTargets ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS upnpQtBase ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES ${upnpQtBase_CamelCase_HEADERS} DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/UPNPQTBASE/UPNPQTBASE COMPONENT Devel)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/upnpQtBase_export.h
  ${upnpQtBase_HEADERS}
  DESTINATION  ${KDE_INSTALL_INCLUDEDIR_KF5}/UPNPQTBASE/upnpQtBase COMPONENT Devel
)

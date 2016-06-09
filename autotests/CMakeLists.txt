include(ECMMarkAsTest)

set(CMAKE_CXX_STANDARD 11)

set(ssdpTests_SRCS
    ssdptests.cpp
)

set(ssdpSocketTests_SRCS
    ssdpsockettests.cpp
)

set(upnpdevicemodeltests_SRCS
    upnpdevicemodeltests.cpp
)

if (Qt5Test_FOUND)
    add_executable(ssdpTests ${ssdpTests_SRCS})
    target_link_libraries(ssdpTests Qt5::Test Qt5::Core Qt5::Network upnpQt)
    add_test(NAME ssdpUnitTest COMMAND ssdpTests)

    add_executable(ssdpSocketTests ${ssdpSocketTests_SRCS})
    target_link_libraries(ssdpSocketTests Qt5::Test Qt5::Core Qt5::Network)

    # add_executable(upnpdevicemodeltests ${upnpdevicemodeltests_SRCS})
    # target_link_libraries(upnpdevicemodeltests Qt5::Test Qt5::Core Qt5::Network upnpQt)
endif()

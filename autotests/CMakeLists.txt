# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

# SPDX-License-Identifier: BSD-2-Clause

include(ECMMarkAsTest)

set(ssdpTests_SRCS
    ssdptests.cpp
)

if (Qt5Test_FOUND)
    add_executable(ssdpTests ${ssdpTests_SRCS})
    target_link_libraries(ssdpTests Qt5::Test Qt5::Core Qt5::Network UpnpLibQt)
    add_test(NAME ssdpUnitTest COMMAND ssdpTests)
endif()

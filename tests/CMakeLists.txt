# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

# SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL

set(ssdplistener_SRCS
    ssdplistener.cpp
)

add_executable(ssdpListener ${ssdplistener_SRCS})
target_link_libraries(ssdpListener Qt5::Core UpnpLibQt)


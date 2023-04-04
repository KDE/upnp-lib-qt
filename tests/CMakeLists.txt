# SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

# SPDX-License-Identifier: BSD-2-Clause

set(ssdplistener_SRCS
    ssdplistener.cpp
)

add_executable(ssdpListener ${ssdplistener_SRCS})
target_link_libraries(ssdpListener Qt::Core UpnpLibQt)


/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpssdpengine.h"

#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QCoreApplication app{argc, argv};

    UpnpSsdpEngine ssdpEngine;

    return app.exec();
}

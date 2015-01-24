/*
 * Copyright 2015 Matthieu Gallien <matthieu_gallien@yahoo.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include <QtCore/QCoreApplication>

#include <QtCore/QDebug>

#include "upnp.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    int res = UpnpInit2(nullptr, 0);
    if (res == UPNP_E_SUCCESS) {
        qDebug() << "success";
    } else {
        qDebug() << "fail";
    }

    res = UpnpFinish();
    if (res == UPNP_E_SUCCESS) {
        qDebug() << "success";
        return 0;
    } else {
        qDebug() << "fail";
        return 1;
    }

    return 0;
}

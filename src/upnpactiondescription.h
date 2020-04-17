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

#ifndef UPNPACTIONDESCRIPTION_H
#define UPNPACTIONDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QString>
#include <QVector>

enum class UpnpArgumentDirection {
    In,
    Out,
    Invalid,
};

class UPNPLIBQT_EXPORT UpnpActionArgumentDescription
{
public:
    UpnpActionArgumentDescription();

    bool mIsValid;

    QString mName;

    UpnpArgumentDirection mDirection;

    bool mIsReturnValue;

    QString mRelatedStateVariable;
};

class UPNPLIBQT_EXPORT UpnpActionDescription
{
public:
    UpnpActionDescription();

    bool mIsValid;

    QString mName;

    QVector<UpnpActionArgumentDescription> mArguments;

    int mNumberInArgument;

    int mNumberOutArgument;
};

#endif // UPNPACTIONDESCRIPTION_H

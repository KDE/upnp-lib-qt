/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
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
    bool mIsValid = false;

    QString mName;

    UpnpArgumentDirection mDirection = UpnpArgumentDirection::Invalid;

    bool mIsReturnValue = false;

    QString mRelatedStateVariable;
};

class UPNPLIBQT_EXPORT UpnpActionDescription
{
public:
    bool mIsValid = false;

    QString mName;

    QVector<UpnpActionArgumentDescription> mArguments;

    int mNumberInArgument = 0;

    int mNumberOutArgument = 0;
};

#endif // UPNPACTIONDESCRIPTION_H

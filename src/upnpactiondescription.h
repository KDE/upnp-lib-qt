/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPACTIONDESCRIPTION_H
#define UPNPACTIONDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QString>
#include <QVector>

/**
 * @brief The UpnpArgumentDirection enum indicates if an argument is sent with the request (In) or received with the answer (Out)
 */
enum class UpnpArgumentDirection {
    In,
    Out,
    Invalid,
};

/**
 * @brief The UpnpActionArgumentDescription class is used to describe one argument of an UPnP action that can be called on a service see \class UpnpAbstractService and \class UpnpActionDescription.
 */
class UPNPLIBQT_EXPORT UpnpActionArgumentDescription
{
public:
    bool mIsValid = false;

    /**
     * @brief mName is the name of the argument
     */
    QString mName;

    /**
     * @brief mDirection indicates if the argument is an input (In or a parameter) or an output (Out or a result).
     */
    UpnpArgumentDirection mDirection = UpnpArgumentDirection::Invalid;

    bool mIsReturnValue = false;

    QString mRelatedStateVariable;
};

/**
 * @brief The UpnpActionDescription class is used to describe one UPnP action that can be called on a service see \class UpnpAbstractService and \class UpnpActionArgumentDescription.
 */
class UPNPLIBQT_EXPORT UpnpActionDescription
{
public:
    bool mIsValid = false;

    /**
     * @brief mName is the name of the action
     */
    QString mName;

    QVector<UpnpActionArgumentDescription> mArguments;

    int mNumberInArgument = 0;

    int mNumberOutArgument = 0;
};

#endif // UPNPACTIONDESCRIPTION_H

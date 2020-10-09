/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPSTATEVARIABLEDESCRIPTION_H
#define UPNPSTATEVARIABLEDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QByteArray>
#include <QString>
#include <QVariant>
#include <QVector>

class QObject;

/**
 * @brief The UpnpStateVariableDescription class provides tyhe description of a state variable of an UPnP service.
 *
 * A service can provides a number of state variables given information about the state of the service. The class \class UpnpStateVariableDescription documents all properties about one state variable.
 */
class UPNPLIBQT_EXPORT UpnpStateVariableDescription
{
public:
    UpnpStateVariableDescription();

    bool mIsValid{false};

    QString mUpnpName;

    QObject *mObject{nullptr};

    QByteArray mPropertyName;

    int mPropertyIndex{-1};

    bool mEvented{false};

    QString mDataType;

    QVariant mDefaultValue;

    QVariant mMinimumValue;

    QVariant mMaximumValue;

    QVariant mStep;

    QVector<QString> mValueList;
};

#endif // UPNPSTATEVARIABLEDESCRIPTION_H

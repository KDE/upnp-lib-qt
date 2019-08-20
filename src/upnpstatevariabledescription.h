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

#ifndef UPNPSTATEVARIABLEDESCRIPTION_H
#define UPNPSTATEVARIABLEDESCRIPTION_H

#include "upnplibqt_export.h"

#include <QtCore/QString>
#include <QtCore/QByteArray>
#include <QtCore/QVariant>
#include <QtCore/QVector>

class QObject;

class UPNPLIBQT_EXPORT UpnpStateVariableDescription
{
public:

    UpnpStateVariableDescription();

    bool mIsValid;

    QString mUpnpName;

    QObject *mObject;

    QByteArray mPropertyName;

    int mPropertyIndex;

    bool mEvented;

    QString mDataType;

    QVariant mDefaultValue;

    QVariant mMinimumValue;

    QVariant mMaximumValue;

    QVariant mStep;

    QVector<QString> mValueList;
};

#endif // UPNPSTATEVARIABLEDESCRIPTION_H

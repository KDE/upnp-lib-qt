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

#ifndef UPNPBASICTYPES_H
#define UPNPBASICTYPES_H

#include <QtCore/QtGlobal>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVariant>

typedef quint32 A_ARG_TYPE_InstanceID;

typedef QString AVTransportURI;

typedef QString AVTransportURIMetaData;

typedef QString A_ARG_TYPE_SeekMode;

typedef QString A_ARG_TYPE_SeekTarget;

enum class UpnpArgumentDirection
{
    In,
    Out,
};

class UpnpActionArgumentDescription
{
public:

    QString mName;

    UpnpArgumentDirection mDirection;

    bool mIsReturnValue;

    QString mRelatedStateVariable;
};

class UpnpActionDescription
{
public:

    QString mName;

    QList<UpnpActionArgumentDescription> mArguments;
};

class UpnpStateVariableDescription
{
public:

    QString mName;

    bool mEvented;

    QString mDataType;

    QVariant mDefaultValue;

    QVariant mMinimumValue;

    QVariant mMaximumValue;

    QVariant mStep;

    QList<QString> mValueList;
};

#endif // UPNPBASICTYPES_H


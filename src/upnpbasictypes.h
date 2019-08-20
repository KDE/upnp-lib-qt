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

#include "upnplibqt_export.h"

#include <QtGlobal>
#include <QString>
#include <QList>
#include <QVariant>

/**
 * A_ARG_TYPE_InstanceID argument type for UPnP method calls matching an unsigned int 32 bits
 */
using A_ARG_TYPE_InstanceID = quint32;

using AVTransportURI = QString;

using AVTransportURIMetaData = QString;

using A_ARG_TYPE_SeekMode = QString;

using A_ARG_TYPE_SeekTarget = QString;

#endif // UPNPBASICTYPES_H


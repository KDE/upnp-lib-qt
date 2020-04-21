/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#ifndef UPNPBASICTYPES_H
#define UPNPBASICTYPES_H

#include "upnplibqt_export.h"

#include <QList>
#include <QString>
#include <QVariant>
#include <QtGlobal>

/**
 * A_ARG_TYPE_InstanceID argument type for UPnP method calls matching an unsigned int 32 bits
 */
using A_ARG_TYPE_InstanceID = quint32;

using AVTransportURI = QString;

using AVTransportURIMetaData = QString;

using A_ARG_TYPE_SeekMode = QString;

using A_ARG_TYPE_SeekTarget = QString;

#endif // UPNPBASICTYPES_H

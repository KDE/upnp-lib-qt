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

#include "upnpservereventobject.h"

#include <QtCore/QDebug>

UpnpServerEventObject::UpnpServerEventObject(QObject *parent) : QObject(parent), KDSoapServerObjectInterface()
{

}

UpnpServerEventObject::~UpnpServerEventObject()
{

}

void UpnpServerEventObject::processRequest(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction)
{
    qDebug() << "UpnpServerEventObject::processRequest";
}

QIODevice *UpnpServerEventObject::processFileRequest(const QString &path, QByteArray &contentType)
{
    qDebug() << "UpnpServerEventObject::processFileRequest";

    return nullptr;
}

void UpnpServerEventObject::processRequestWithPath(const KDSoapMessage &request, KDSoapMessage &response, const QByteArray &soapAction, const QString &path)
{
    qDebug() << "UpnpServerEventObject::processRequestWithPath";
}

bool UpnpServerEventObject::processCustomVerbRequest(const QByteArray &requestData, const QMap<QByteArray, QByteArray> &headers)
{
    qDebug() << "UpnpServerEventObject::processCustomVerbRequest" << requestData << headers;

    return true;
}

#include "moc_upnpservereventobject.cpp"

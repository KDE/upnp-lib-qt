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

#ifndef UPNPCONTROLCONTENTDIRECTORY_H
#define UPNPCONTROLCONTENTDIRECTORY_H

#include "upnpQt_export.h"

#include "upnpcontrolabstractservice.h"
#include "upnpbasictypes.h"

class UpnpControlContentDirectoryPrivate;

class UPNPQT_EXPORT UpnpControlContentDirectory : public UpnpControlAbstractService
{
    Q_OBJECT

    Q_PROPERTY(QString searchCapabilities
               READ searchCapabilities
               NOTIFY searchCapabilitiesChanged)

    Q_PROPERTY(QString transferIDs
               READ transferIDs
               NOTIFY transferIDsChanged)

    Q_PROPERTY(bool hasTransferIDs
               READ hasTransferIDs
               NOTIFY hasTransferIDsChanged)

    Q_PROPERTY(QString sortCapabilities
               READ sortCapabilities
               NOTIFY sortCapabilitiesChanged)

    Q_PROPERTY(int systemUpdateID
               READ systemUpdateID
               NOTIFY systemUpdateIDChanged)

public:

    explicit UpnpControlContentDirectory(QObject *parent = 0);

    virtual ~UpnpControlContentDirectory();

    const QString& searchCapabilities() const;

    const QString& transferIDs() const;

    bool hasTransferIDs() const;

    const QString& sortCapabilities() const;

    int systemUpdateID() const;

public Q_SLOTS:

    void getSearchCapabilities();

    void getSortCapabilities();

    void getSystemUpdateID();

    void search(const QString &objectID, const QString &searchCriteria,
                const QString &filter, int startingIndex,
                int requestedCount, const QString &sortCriteria);

    void browse(const QString &objectID, const QString &browseFlag,
                const QString &filter, int startingIndex,
                int requestedCount, const QString &sortCriteria);

Q_SIGNALS:

    void searchCapabilitiesChanged(const QString &capabilities);

    void transferIDsChanged(const QString &ids);

    void hasTransferIDsChanged();

    void sortCapabilitiesChanged(const QString &capabilities);

    void systemUpdateIDChanged(int id);

    void getSearchCapabilitiesFinished(const QString &searchCaps, bool success);

    void getSortCapabilitiesFinished(const QString &sortCaps, bool success);

    void getSystemUpdateIDFinished(int id, bool success);

    void searchFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success);

    void browseFinished(const QString &result, int numberReturned, int totalMatches, int systemUpdateID, bool success);

private Q_SLOTS:

    void finishedGetSearchCapabilitiesCall(KDSoapPendingCallWatcher *self);

    void finishedGetSortCapabilitiesCall(KDSoapPendingCallWatcher *self);

    void finishedGetSystemUpdateIDCall(KDSoapPendingCallWatcher *self);

    void finishedSearchCall(KDSoapPendingCallWatcher *self);

    void finishedBrowseCall(KDSoapPendingCallWatcher *self);

protected:

    void parseServiceDescription(QIODevice *serviceDescriptionContent) override;

    void parseEventNotification(const QString &eventName, const QString &eventValue) Q_DECL_OVERRIDE;

private:

    UpnpControlContentDirectoryPrivate *d;

};

#endif // UPNPCONTROLCONTENTDIRECTORY_H


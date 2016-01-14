#include "upnpwebsocketdevicemodel.h"

#include "upnpwebsocketclient.h"
#include "upnpdevicedescription.h"

UpnpWebSocketDeviceModel::UpnpWebSocketDeviceModel(QObject *parent)
    : QAbstractListModel(parent), mWebSocketClient(nullptr), mListDevices(), mDevices()
{   
}

UpnpWebSocketDeviceModel::~UpnpWebSocketDeviceModel()
{
}

int UpnpWebSocketDeviceModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }

    return mListDevices.count();
}

QVariant UpnpWebSocketDeviceModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return {};
    }

    if (index.row() < 0 || index.row() > mListDevices.count() - 1) {
        return {};
    }

    if (role < ColumnsRoles::NameRole || role > ColumnsRoles::UDNRole) {
        return QVariant();
    }

    ColumnsRoles convertedRole = static_cast<ColumnsRoles>(role);

    switch(convertedRole)
    {
    case ColumnsRoles::NameRole:
        return mDevices[mListDevices.at(index.row())]->friendlyName();
    case ColumnsRoles::UDNRole:
        return mDevices[mListDevices.at(index.row())]->UDN();
    }

    return {};
}

QHash<int, QByteArray> UpnpWebSocketDeviceModel::roleNames() const
{
    return {{NameRole, {"name"}}, {UDNRole, {"udn"}}};
}

void UpnpWebSocketDeviceModel::setWebSocketClient(UpnpWebSocketClient *value)
{
    if (mWebSocketClient) {
        disconnect(mWebSocketClient, &UpnpWebSocketClient::newDevice, this, &UpnpWebSocketDeviceModel::newDevice);
        disconnect(mWebSocketClient, &UpnpWebSocketClient::removedDevice, this, &UpnpWebSocketDeviceModel::removedDevice);
    }
    mWebSocketClient = value;

    connect(mWebSocketClient, &UpnpWebSocketClient::newDevice, this, &UpnpWebSocketDeviceModel::newDevice);
    connect(mWebSocketClient, &UpnpWebSocketClient::removedDevice, this, &UpnpWebSocketDeviceModel::removedDevice);

    Q_EMIT webSocketClientChanged();
}

UpnpWebSocketClient *UpnpWebSocketDeviceModel::webSocketClient() const
{
    return mWebSocketClient;
}

void UpnpWebSocketDeviceModel::newDevice(const QString &udn)
{
    auto itDevice = mDevices.find(udn);

    if (itDevice != mDevices.end()) {
        return;
    }

    beginInsertRows({}, mListDevices.count(), mListDevices.count());
    mListDevices.push_back(udn);
    mDevices[udn] = mWebSocketClient->device(udn);
    endInsertRows();
}

void UpnpWebSocketDeviceModel::removedDevice(const QString &udn)
{
    auto itDevice = mDevices.find(udn);

    if (itDevice == mDevices.end()) {
        return;
    }

    auto itUdn = mListDevices.indexOf(udn);
    beginRemoveRows({}, itUdn, itUdn);
    mListDevices.removeAt(itUdn);
    mDevices.erase(itDevice);
    endRemoveRows();
}


#include "moc_upnpwebsocketdevicemodel.cpp"

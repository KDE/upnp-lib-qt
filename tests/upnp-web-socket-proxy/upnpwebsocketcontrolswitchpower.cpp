#include "upnpwebsocketcontrolswitchpower.h"

UpnpWebSocketControlSwitchPower::UpnpWebSocketControlSwitchPower(QObject *parent)
    : UpnpWebSocketAbstractServiceControl(parent)
{
}

UpnpWebSocketControlSwitchPower::~UpnpWebSocketControlSwitchPower()
{
}

void UpnpWebSocketControlSwitchPower::setTarget(bool newTargetValue)
{
    callAction(QStringLiteral("SetTarget"), {newTargetValue});
}

void UpnpWebSocketControlSwitchPower::getTarget()
{
    callAction(QStringLiteral("GetTarget"), {});
}

void UpnpWebSocketControlSwitchPower::getStatus()
{
    callAction(QStringLiteral("GetStatus"), {});
}


#include "moc_upnpwebsocketcontrolswitchpower.cpp"

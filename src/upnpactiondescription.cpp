/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpactiondescription.h"

UpnpActionArgumentDescription::UpnpActionArgumentDescription()
    : mIsValid(false)
    , mName()
    , mDirection(UpnpArgumentDirection::Invalid)
    , mIsReturnValue(false)
    , mRelatedStateVariable()
{
}

UpnpActionDescription::UpnpActionDescription()
    : mIsValid(false)
    , mName()
    , mArguments()
    , mNumberInArgument(0)
    , mNumberOutArgument(0)
{
}

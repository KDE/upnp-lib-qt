/*
   SPDX-FileCopyrightText: 2015 (c) Matthieu Gallien <matthieu_gallien@yahoo.fr>

   SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "upnpstatevariabledescription.h"

UpnpStateVariableDescription::UpnpStateVariableDescription()
    : mIsValid(false)
    , mUpnpName()
    , mObject(nullptr)
    , mPropertyName()
    , mPropertyIndex(-1)
    , mEvented(false)
    , mDataType()
    , mDefaultValue()
    , mMinimumValue()
    , mMaximumValue()
    , mStep()
    , mValueList()
{
}

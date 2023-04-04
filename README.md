<!--
SPDX-FileCopyrightText: 2020 Matthieu Gallien <matthieu_gallien@yahoo.fr>

SPDX-License-Identifier: CC0-1.0
-->

**UPnP Qt Library**
==============

Project to build a library to implement support for the [UPnP Specification](https://openconnectivity.org/developer/specifications/upnp-resources/upnp/).

Currently provide basic building blocks with support for discovery protocol SSDP and the UPnP control protocol built using SOAP.

The SOAP part is done via the [KDSoap](https://www.kdab.com/development-resources/qt-tools/kd-soap/) library from KDAB.

## Compilation

### Dependencies

**upnp-lib-qt** depends on the following libaries:

 * Qt >= 6.4
 * CMake >= 3.8
 * CMake Extra Modules >= 5.59.0
 * KDSoap >= 1.5.0
 
### Building

**upnp-lib-qt** can be built with:

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

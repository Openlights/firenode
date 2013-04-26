// FireNode
// Copyright (c) 2013 Jon Evans
// http://craftyjon.com/projects/openlights/firenode
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <stdio.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>

#include "portability.h"
#include "version.h"
#include "networking.h"
#include "unpacker.h"
#include "serial.h"


int main(int argc, char** argv)
{
    
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();

    QString com_port;
    uint16_t udp_port = 65535;

    if (args.size() < 3) {
        qDebug() << "Too few arguments.";
        qDebug() << "Usage: firenode --serial=<COM_PORT> --udp=<UDP_PORT>";
        return 1;
    }

    QRegExp rx_arg_com_port("--serial=([0-9a-zA-Z/]+)");
    QRegExp rx_arg_udp_port("--udp=([0-9]+)");

    for (int i = 1; i < args.size(); i++) {
        if (rx_arg_com_port.indexIn(args.at(i)) != -1) {
            com_port = rx_arg_com_port.cap(1);
        }
        if (rx_arg_udp_port.indexIn(args.at(i)) != -1) {
            udp_port = rx_arg_udp_port.cap(1).toInt();
        }
    }

    if (com_port == "" || udp_port == 65535) {
        qDebug() << "Invalid arguments.";
        qDebug() << "Usage: firenode --serial=<COM_PORT> --udp=<UDP_PORT>";
        return 1;
    }

    qDebug("FireNode %d.%d.%d starting up...", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

    Networking net(udp_port);
    Unpacker up;
    Serial ser(com_port);

    QObject::connect(&net, SIGNAL(data_ready(QByteArray*)), &up, SLOT(unpack_data(QByteArray*)));
    QObject::connect(&up, SIGNAL(data_ready(QByteArray*)), &ser, SLOT(write_data(QByteArray*)));

    qDebug() << "Listening on UDP" << udp_port << "and transmitting on serial port" << com_port;

    return app.exec();
}

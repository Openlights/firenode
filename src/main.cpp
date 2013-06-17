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

#include <cstdio>
#include <iostream>
#include <csignal>

#include <QtCore/QCoreApplication>
#include <QtCore/QObject>
#include <QtCore/QRegExp>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QThread>

#include "portability.h"
#include "version.h"
#include "networking.h"
#include "unpacker.h"
#include "serial.h"
#include "usb.h"


QCoreApplication *pApp;


void sig_handler(int sig)
{
    if (sig == SIGINT || sig == SIGTERM)
    {
        pApp->quit();
    }
}


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    pApp = &app;

    QStringList args = app.arguments();

    uint16_t udp_port = 65535;
    QString serial_port;

    if (args.size() < 3) {
        qDebug() << "Too few arguments.";
        qDebug() << "Usage: firenode --udp=<UDP_PORT> --serial=<SERIAL_DEVICE>";
        return 1;
    }

    QRegExp rx_arg_udp_port("--udp=([0-9]+)");
    QRegExp rx_arg_serial_port("--serial=([0-9a-zA-Z/]+)");

    for (int i = 1; i < args.size(); i++) {
        if (rx_arg_udp_port.indexIn(args.at(i)) != -1) {
            udp_port = rx_arg_udp_port.cap(1).toInt();
        }

        if (rx_arg_serial_port.indexIn(args.at(i)) != -1) {
            serial_port = rx_arg_serial_port.cap(1);
        }
    }

    if (udp_port == 65535) {
        qDebug() << "Invalid arguments.";
        qDebug() << "Usage: firenode --udp=<UDP_PORT>";
        return 1;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    qDebug("FireNode %d.%d.%d starting up...", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

    Networking net(udp_port);
    Unpacker up;
    //USBStrandController sc;
    Serial *ser = new Serial(serial_port);

    //sc.connect();

    // 0x99, 0x00, 0x15, 0x02, 0x00, 0xA0, 0x00, 0xA0

    QByteArray *init_strands = new QByteArray();

    init_strands->append((char)0x99);
    init_strands->append((char)0x00);
    init_strands->append((char)0x00);
    init_strands->append((char)0x15);
    init_strands->append((char)0x02);
    init_strands->append((char)0x00);
    init_strands->append((char)0xA0);
    init_strands->append((char)0x00);
    //init_strands->append((char)0xA0);

    ser->write_data(init_strands);

    delete init_strands;

    //QObject::connect(serial, SIGNAL(started()), ser, SLOT(start_timer()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), ser, SLOT(quit()));

    ser->start();

    QTimer stats_timer;
    stats_timer.start((unsigned int)(1000.0 * STATS_TIME));

    QObject::connect(&net, SIGNAL(data_ready(QByteArray*)), &up, SLOT(unpack_data(QByteArray*)));
    //QObject::connect(&up, SIGNAL(data_ready(QByteArray*)), &sc, SLOT(write_data(QByteArray*)));
    QObject::connect(&up, SIGNAL(data_ready(QByteArray*)), ser, SLOT(enqueue_data(QByteArray*)));
    QObject::connect(&up, SIGNAL(packet_start()), ser, SLOT(packet_start()));
    QObject::connect(&up, SIGNAL(packet_done()), ser, SLOT(packet_done()));

    //QObject::connect(&stats_timer, SIGNAL(timeout()), ser, SLOT(print_stats()));

    qDebug() << "Listening on UDP" << udp_port;



    app.exec();

    std::cout << "Bye" << std::endl;

    return 0;
}

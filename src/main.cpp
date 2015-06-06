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
    QString serial_port;
    int first_strand = -1, last_strand = -1;

    if (args.size() < 4) {
        qDebug() << "Too few arguments.";
        qDebug() << "Usage: firenode --serial=<SERIAL_DEVICE> --first-strand=N --last-strand=M";
        return 1;
    }

    QRegExp rx_arg_first_strand("--first-strand=([0-9]+)");
    QRegExp rx_arg_last_strand("--last-strand=([0-9]+)");
    QRegExp rx_arg_serial_port("--serial=([0-9a-zA-Z/]+)");

    for (int i = 1; i < args.size(); i++) {
        if (rx_arg_serial_port.indexIn(args.at(i)) != -1) {
            serial_port = rx_arg_serial_port.cap(1);
        }

        if (rx_arg_first_strand.indexIn(args.at(i)) != -1) {
            first_strand = rx_arg_first_strand.cap(1).toInt();
        }

        if (rx_arg_last_strand.indexIn(args.at(i)) != -1) {
            last_strand = rx_arg_last_strand.cap(1).toInt();
        }
    }

    if ((first_strand < 0) ||
        (last_strand < 0) ||
        (first_strand > last_strand) ||
        ((last_strand - first_strand) > 8)) {
        qDebug("Invalid strand spec: %d to %d.", first_strand, last_strand);
        qDebug() << "Usage: firenode --serial=<SERIAL_DEVICE> --first-strand=N --last-strand=M";
        return 1;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    qDebug("FireNode %d.%d.%d starting up...", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

    Networking net(3020);
    Unpacker up(first_strand, last_strand);
    Serial *ser = new Serial(serial_port);

    //QObject::connect(serial, SIGNAL(started()), ser, SLOT(start_timer()));



    //QTimer stats_timer;
    //stats_timer.start((unsigned int)(1000.0 * STATS_TIME));

    QObject::connect(&net, SIGNAL(data_ready(QByteArray*)), &up, SLOT(unpack_data(QByteArray*)));
    QObject::connect(&up, SIGNAL(frame_end()), &up, SLOT(assemble_data()));
    QObject::connect(&up, SIGNAL(data_ready(QByteArray*)), ser, SLOT(write_data(QByteArray*)));

    QThread netThread;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &netThread, SLOT(quit()));

    netThread.start();
    net.moveToThread(&netThread);
    net.start();

    //QObject::connect(&stats_timer, SIGNAL(timeout()), ser, SLOT(print_stats()));

    app.exec();

    std::cout << "Bye" << std::endl;

    return 0;
}

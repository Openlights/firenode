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
#include <QtCore/QFile>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

#include "portability.h"
#include "version.h"
#include "networking.h"
#include "unpacker.h"
#include "serial.h"


QCoreApplication *pApp;

#define MAX_OUTPUTS 256


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

    QFile config_file("config.json");

    if (!config_file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open config.json, cannot continue");
        return 1;
    }

    QByteArray config_data = config_file.readAll();
    QJsonDocument config_doc(QJsonDocument::fromJson(config_data));

    int udp_port = config_doc.object()["port"].toInt();

    QJsonArray outputs = config_doc.object()["outputs"].toArray();

    Serial* serials[MAX_OUTPUTS];
    Unpacker* unpackers[MAX_OUTPUTS];

    int num_serials = 0;

    if (outputs.size() > MAX_OUTPUTS) {
        qWarning("Cannot have more than %d output devices.", MAX_OUTPUTS);
        return 2;
    }

    Networking net(udp_port);
    QTimer *serial_timer = new QTimer(&app);
    serial_timer->setInterval(1.0 / 25.0);

    for (int output_index = 0; output_index < outputs.size(); output_index++) {
        QJsonObject output_obj = outputs[output_index].toObject();
        //qDebug() << output_index << output_obj;

        QString serial_port = output_obj["port"].toString();
        int first_strand = output_obj["first-strand"].toInt();
        int last_strand = output_obj["last-strand"].toInt();

        serials[output_index] = new Serial(serial_port);
        unpackers[output_index] = new Unpacker(first_strand, last_strand);
        num_serials++;

        QObject::connect(&net, SIGNAL(data_ready(QByteArray)), unpackers[output_index], SLOT(unpack_data(QByteArray)));
        QObject::connect(unpackers[output_index], SIGNAL(frame_end()), unpackers[output_index], SLOT(assemble_data()));
        QObject::connect(unpackers[output_index], SIGNAL(data_ready(QByteArray*)), serials[output_index], SLOT(update_data(QByteArray*)));
        QObject::connect(serial_timer, SIGNAL(timeout()), serials[output_index], SLOT(write_data()));
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    qDebug("FireNode %d.%d.%d starting up...", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD);

    //QTimer stats_timer;
    //stats_timer.start((unsigned int)(1000.0 * STATS_TIME));

    QThread netThread;
    QObject::connect(&app, SIGNAL(aboutToQuit()), &net, SLOT(stop()));
    QObject::connect(&app, SIGNAL(aboutToQuit()), &netThread, SLOT(quit()));

    netThread.start();
    net.moveToThread(&netThread);
    net.start();

    //QThread timer_thread;
    //serial_timer->moveToThread(&timer_thread);
    serial_timer->start();
    //timer_thread.start();

#ifdef USE_ZMQ
    QTimer *net_timer = new QTimer(&app);
    net_timer->setInterval(1.0);
    QObject::connect(net_timer, SIGNAL(timeout()), &net, SLOT(get_data()));
    net_timer->start();
#endif

    //QObject::connect(&stats_timer, SIGNAL(timeout()), ser, SLOT(print_stats()));

    app.exec();

    serial_timer->deleteLater();

    for (int serial_index = 0; serial_index < num_serials; serial_index++) {
        serials[serial_index]->deleteLater();
        unpackers[serial_index]->deleteLater();
    }

    std::cout << "Bye" << std::endl;

    return 0;
}

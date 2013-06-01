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

#ifndef _SERIAL_H
#define _SERIAL_H

#include "portability.h"

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QtCore/QQueue>
#include <QtCore/QTimer>


#define STATS_TIME 1.0


//! Writes data to strand controller connected to a virtual serial port.
class Serial : public QThread
{
    Q_OBJECT

public:
    Serial(const QString name);
    ~Serial();

    unsigned long long get_pps_and_reset(void);
    void run(void);

public slots:
	void write_data(QByteArray *data);
    void enqueue_data(QByteArray *data);
    void print_stats(void);
    void process_loop(void);
    void start_timer(void);
    void shutdown(void);
    void packet_start(void);
    void packet_done(void);

signals:
    void data_written(); 

private:
    QSerialPort _port;
    QTimer *_timer;
    bool _packet_in_process;

    unsigned long long _packets;
    bool _exit;
    QQueue<QByteArray> _q;
    QByteArray _last_packet;
};

#endif

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

#include "serial.h"


Serial::Serial(const QString name)
{
    //QSerialPortInfo info = QSerialPortInfo(name);
    _packets = 0;
    _port.close();
    _port.setPortName(name);

    if (!_port.open(QIODevice::ReadWrite)) {
        qDebug() << "Could not open port, code" << _port.error();
    }

    //if (!_port.setBaudRate(2000000)) {
    if (!_port.setBaudRate(2000000)) {
        qDebug() << "Error setting baud rate, code" << _port.error();
        qDebug() << "Current rate is" << _port.baudRate();
    }

    if (!_port.setDataBits(QSerialPort::Data8)) {
        qDebug() << "Error setting up port databits, code" << _port.error();
    }

    _port.setParity(QSerialPort::NoParity);
    _port.setStopBits(QSerialPort::OneStop);
    _port.setFlowControl(QSerialPort::NoFlowControl);

    _exit = false;
    _packet_in_process = false;
    _pending_write = false;
}


Serial::~Serial()
{
    _port.close();
    if (_timer) delete _timer;
}

void Serial::packet_start()
{
    _packet_in_process = true;
    _q.clear();
}


void Serial::packet_done()
{
    _packet_in_process = false;
    _pending_write = true;
}


void Serial::shutdown()
{
    _exit = true;
}

#if 0
void Serial::start_timer()
{
    _timer = new QTimer();
    _timer->setInterval(3);
    connect(_timer, SIGNAL(timeout()), this, SLOT(process_loop()));
    _timer->start();
}


void Serial::process_loop()
{
    if (!_q.isEmpty() && !_packet_in_process)
    {
        QByteArray d = _q.dequeue();
        write_data(&d);
        _pending_write = false;
    }
}
#endif

void Serial::write_data(QByteArray *data)
{
    //qDebug() << "write_data";
    //_port.clear();
    char reply[256];

    /*QByteArray test;
    test.append('*');
    test.append('\0');
    test.append('\0');
    for (int i = 0; i < 240 * 3 * 8; i++) {
        test.append('\x00');
    }*/

    int rc = _port.write(*data);
    if (rc < 0) {
        qDebug() << "Write error";
    }
#if 0
    else {
        qDebug("wrote %d bytes", rc);
    }
#endif

    if (!_port.waitForBytesWritten(30)) {
        qDebug() << "Timeout!";
    }
#if 0
    if (_port.waitForReadyRead(1000)) {
        _port.read(reply, 256);
        qDebug() << "Reply:" << reply;
    }
#endif
    _packets++;
    _last_packet = *data;

    //_port.flush();
}

#if 0
void Serial::enqueue_data(QByteArray *data, bool force)
{
    // Hack.
    if (force || !_pending_write)
        _q.enqueue(*data);
}


unsigned long long Serial::get_pps_and_reset()
{
    unsigned long long p = _packets;
    _packets = 0;
    return p;
}


void Serial::print_stats()
{
    unsigned long long packets = get_pps_and_reset();
    qDebug("%0.2f packets/sec, _last_packet.size = %d", (double)packets / STATS_TIME, _last_packet.size());
    qDebug() << _last_packet.toHex();

}
#endif

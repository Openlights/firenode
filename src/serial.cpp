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

    _packet_start_frame = QByteArray();
    _packet_end_frame = QByteArray();

    _packet_start_frame.append((char)0x99);
    _packet_start_frame.append((char)0x00);
    _packet_start_frame.append((char)0x00);
    _packet_start_frame.append((char)0x30);
    _packet_start_frame.append((char)0x01);
    _packet_start_frame.append((char)0x00);
    _packet_start_frame.append((char)0x00);

    _packet_end_frame.append((char)0x99);
    _packet_end_frame.append((char)0x00);
    _packet_end_frame.append((char)0x00);
    _packet_end_frame.append((char)0x31);
    _packet_end_frame.append((char)0x01);
    _packet_end_frame.append((char)0x00);
    _packet_end_frame.append((char)0x00);

}


Serial::~Serial()
{
    _port.close();
    if (_timer) delete _timer;
}


void Serial::run()
{
    start_timer();
    exec();
}


void Serial::packet_start()
{
    _packet_in_process = true;
    send_sof();
}


void Serial::packet_done()
{
    send_eof();
    _packet_in_process = false;
}


void Serial::shutdown()
{
    _exit = true;
}


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
    }
}


void Serial::write_data(QByteArray *data)
{
    _port.clear();

    if (!_port.write(*data)) {
        qDebug() << "Write error";
    }

    if (!_port.waitForBytesWritten(30)) {
        qDebug() << "Timeout!";
    }

    //qDebug() << data->toHex();

    _packets++;
    _last_packet = *data;

    _port.flush();
}


void Serial::send_sof()
{
    enqueue_data(&_packet_start_frame);
}


void Serial::send_eof()
{
    enqueue_data(&_packet_end_frame);
}


void Serial::enqueue_data(QByteArray *data)
{
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
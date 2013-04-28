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

    
}


Serial::~Serial()
{
    _port.close();
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

    _port.flush();
}

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

#include "networking.h"


Networking::Networking()
{
    _socket = new QUdpSocket(this);
    _socket->bind(QHostAddress::LocalHost, LISTEN_PORT);

    connect(_socket, SIGNAL(readyRead()), this, SLOT(read_pending_packets()));
}


Networking::~Networking()
{
}


bool Networking::open(void)
{
    return true;    
}


bool Networking::close(void)
{
    return true;
}


void Networking::read_pending_packets()
{
    while (_socket->hasPendingDatagrams())
    {
        QByteArray dgram;
        dgram.resize(_socket->pendingDatagramSize());
        _socket->readDatagram(dgram.data(), dgram.size());

        emit data_ready(&dgram);
    }
}

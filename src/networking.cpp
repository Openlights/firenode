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
//#include "zmq.h"

Networking::Networking(int port)
{
#ifdef USE_ZMQ
    Q_UNUSED(port);
    running = false;

    context = zmq_ctx_new();
    subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://localhost:3020");
    Q_ASSERT(rc == 0);

    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    Q_ASSERT(rc == 0);
#else
    _socket = new QUdpSocket(this);
    _socket->bind(QHostAddress::LocalHost, port, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    qDebug("Listening on port %d", port);

    connect(_socket, SIGNAL(readyRead()), this, SLOT(read_pending_packets()));
#endif
}

void Networking::start()
{
    running = true;
    emit run();
}

void Networking::stop()
{
    qDebug() << "quit!";
    running = false;
}

void Networking::run()
{

}

void Networking::read_pending_packets()
{
    while (_socket->hasPendingDatagrams())
    {
        QByteArray dgram;
        dgram.resize(_socket->pendingDatagramSize());
        _socket->readDatagram(dgram.data(), dgram.size());

        emit data_ready(dgram);
    }
}

void Networking::get_data()
{
#if 1
    char buffer [MAX_PACKET_SIZE + 1];

    bool receivemore = true;
    while (receivemore) {
        int size = zmq_recv(subscriber, buffer, MAX_PACKET_SIZE, 0);

        if (size == -1) {
            return;
        }

        size_t sockoptlen = 0;
        int sockopt = 0;
        zmq_getsockopt(subscriber, ZMQ_RCVMORE, &sockopt, &sockoptlen);
        receivemore = (sockopt == 1);

        if (size > MAX_PACKET_SIZE) {
            size = MAX_PACKET_SIZE;
            qDebug() << "WARNING: had to truncate packet!";
        }

        buffer[size] = 0;

        QByteArray ba(buffer, size);
        emit data_ready(ba);
    }
#else
    QByteArray start_frame("B");
    QByteArray end_frame("E");
    const char strand0_data[] = {83, 0, 208, 2, 206, 255, 175, 204, 255, 173, 203, 255, 170, 200, 255, 165,
                                  198, 255, 163, 197, 255, 160, 195, 255, 158, 193, 255, 155, 190, 255, 150,
                                  189, 255, 147, 187, 255, 145, 186, 255, 142, 184, 255, 140, 182, 255, 137,
                                  179, 255, 132, 178, 255, 130, 176, 255, 127, 175, 255, 124, 173, 255, 122,
                                  171, 255, 119, 178, 255, 130, 182, 255, 137, 187, 255, 145, 193, 255, 155,
                                  198, 255, 163, 204, 255, 173, 209, 255, 181, 214, 255, 188, 220, 255, 198,
                                  225, 255, 206, 231, 255, 216, 236, 255, 224, 240, 255, 232, 247, 255, 242,
                                  251, 255, 249, 1, 5, 0, 4, 12, 0, 7, 20, 0, 11, 30, 0, 14, 38, 0, 18, 48, 0,
                                  21, 56, 0, 24, 63, 0, 28, 73, 0, 31, 81, 0, 35, 91, 0, 38, 99, 0, 41, 107, 0,
                                  45, 117, 0, 48, 124, 0, 52, 135, 0, 55, 142, 0, 58, 150, 0, 61, 160, 0, 64,
                                  168, 0, 68, 178, 0, 71, 186, 0, 74, 193, 0, 78, 204, 0, 81, 211, 0, 85, 221,
                                  0, 88, 229, 0, 91, 237, 0, 92, 239, 0, 91, 237, 0, 90, 234, 0, 89, 232, 0, 88,
                                  229, 0, 87, 226, 0, 85, 221, 0, 84, 219, 0, 83, 216, 0, 82, 214, 0, 81, 211,
                                  0, 79, 206, 0, 78, 204, 0, 77, 201, 0, 76, 198, 0, 75, 196, 0, 74, 193, 0, 72,
                                  188, 0, 71, 186, 0, 70, 183, 0, 69, 181, 0, 68, 178, 0, 66, 173, 0, 65, 170,
                                  0, 64, 168, 0, 63, 165, 0, 62, 163, 0, 61, 160, 0, 59, 155, 0, 58, 153, 0, 58,
                                  150, 0, 57, 147, 0, 56, 145, 0, 54, 140, 0, 53, 137, 0, 52, 135, 0, 51, 132,
                                  0, 50, 130, 0, 49, 127, 0, 47, 122, 0, 46, 119, 0, 45, 117, 0, 44, 114, 0, 43,
                                  112, 0, 45, 117, 0, 49, 127, 0, 52, 135, 0, 55, 142, 0, 58, 153, 0, 61, 160,
                                  0, 65, 170, 0, 68, 178, 0, 71, 186, 0, 75, 196, 0, 78, 204, 0, 82, 214, 0, 88,
                                  229, 0, 85, 221, 0, 88, 229, 0, 91, 237, 0, 95, 247, 0, 98, 255, 0, 103, 255,
                                  7, 107, 255, 15, 113, 255, 25, 118, 255, 33, 123, 255, 40, 129, 255, 51, 134,
                                  255, 58, 139, 255, 66, 145, 255, 76, 150, 255, 84, 154, 255, 91, 160, 255,
                                  101, 165, 255, 109, 170, 255, 117, 176, 255, 127, 181, 255, 135, 186, 255,
                                  142, 192, 255, 153, 197, 255, 160, 201, 255, 168, 207, 255, 178, 212, 255,
                                  186, 217, 255, 193, 223, 255, 203, 228, 255, 211, 233, 255, 219, 239, 255,
                                  229, 239, 255, 229, 237, 255, 226, 236, 255, 224, 234, 255, 221, 233, 255,
                                  219, 231, 255, 216, 228, 255, 211, 226, 255, 209, 225, 255, 206, 223, 255,
                                  203, 222, 255, 201, 218, 255, 196, 217, 255, 193, 215, 255, 191, 214, 255,
                                  188, 212, 255, 186, 209, 255, 181, 207, 255, 178, 206, 255, 175, 204, 255,
                                  173, 203, 255, 170, 200, 255, 165, 198, 255, 163, 197, 255, 160, 195, 255,
                                  158, 193, 255, 155, 192, 255, 153, 189, 255, 147, 187, 255, 145, 186, 255,
                                  142, 184, 255, 140, 182, 255, 137, 179, 255, 132, 178, 255, 130, 176, 255,
                                  127, 175, 255, 124, 173, 255, 122, 170, 255, 117, 168, 255, 114, 167, 255,
                                  112, 165, 255, 109, 164, 255, 107, 160, 255, 101, 159, 255, 99, 159, 255, 99,
                                  164, 255, 107, 170, 255, 117, 175, 255, 124, 179, 255, 132, 186, 255, 142,
                                  190, 255, 150, 195, 255, 158, 201, 255, 168, 206, 255, 175, 211, 255, 183,
                                  217, 255, 193, 222, 255, 201, 226, 255, 209, 233, 255, 219, 237, 255, 226,
                                  242, 255, 234, 248, 255, 244, 253, 255, 252, 1, 5, 0, 4, 12, 0, 8, 22, 0, 11,
                                  30, 0, 14, 38, 0, 18, 48, 0, 21, 56, 0, 24, 63, 0, 28, 73, 0, 31, 81, 0, 34,
                                  89, 0, 38, 99, 0, 41, 107, 0, 44, 114, 0, 48, 124, 0, 51, 132, 0, 54, 140, 0,
                                  58, 150, 0, 60, 158, 0, 63, 165, 0, 67, 175, 0, 70, 183, 0, 73, 191, 0, 77,
                                  201, 0, 83, 216, 0 };

    QByteArray strand0 = QByteArray::fromRawData(strand0_data, sizeof(strand0_data));

    emit data_ready(&start_frame);
    emit data_ready(&strand0);
    //emit data_ready(&strand1);
    //emit data_ready(&strand2);
    //emit data_ready(&strand3);
    //emit data_ready(&strand4);
    //emit data_ready(&strand5);
    //emit data_ready(&strand6);
    //emit data_ready(&strand7);
    emit data_ready(&end_frame);
#endif
}

Networking::~Networking()
{
#ifdef USE_ZMQ
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
#else
    _socket->close();
#endif
}

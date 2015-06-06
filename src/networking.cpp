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
#include "zmq.h"

Networking::Networking(int port)
{
    Q_UNUSED(port);
    running = false;

    context = zmq_ctx_new();
    subscriber = zmq_socket(context, ZMQ_SUB);
    int rc = zmq_connect(subscriber, "tcp://localhost:3020");
    Q_ASSERT(rc == 0);

    rc = zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, "", 0);
    Q_ASSERT(rc == 0);
}

void Networking::start()
{
    running = true;
    emit run();
}

void Networking::run()
{
    char buffer [MAX_PACKET_SIZE + 1];

    while (running)
    {
        int size = zmq_recv(subscriber, buffer, MAX_PACKET_SIZE, 0);
        if (size == -1) {
            continue;
        }

        if (size > MAX_PACKET_SIZE) {
            size = MAX_PACKET_SIZE;
        }

        buffer[size] = 0;

        QByteArray ba(buffer, size);
        emit data_ready(&ba);
    }
}

Networking::~Networking()
{
    zmq_close(subscriber);
    zmq_ctx_destroy(context);
}

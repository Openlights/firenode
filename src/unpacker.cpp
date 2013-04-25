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

#include "unpacker.h"


Unpacker::Unpacker()
{
}


Unpacker::~Unpacker()
{
}


/// Unpacks data from the wire.
/// 
/// Input data format is [CMD, LEN, DATA] where CMD is a one-byte identifier,
/// LEN is a 2-byte (MSB first) length, and DATA is an array of LEN bytes.

void Unpacker::unpack_data(QByteArray *data)
{
    //uint8_t cmd = 0;
    //uint16_t datalen = 0;
    //uint8_t strand_id = 0;

    //QByteArray dbgarr = data->toHex();

    //cmd = data->at(0);
    //datalen = (data->at(1) << 8) + data->at(2);

    //data->remove(0, 3);
    //strand_id = data->at(0);

    //qDebug() << "Command" << cmd << " Length" << datalen;

    //qDebug() << "Strand ID:" << strand_id;

    emit data_ready(data);
}
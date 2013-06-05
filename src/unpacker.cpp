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
#include "colorsys.h"


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
    //qDebug() << "Data length:" << data->length() << ", length bytes = " << data->toHex().at(2) << data->toHex().at(3);

    //qDebug() << data->toHex().left(16);

    // Fix byte order
    QByteArray new_data;
    unsigned short data_len = 0, escapes = 0, packet_size = 0;
    //qDebug() << "Sending packets";
    emit packet_start();

    do {
        //qDebug() << "Data" << data->left(16).toHex();
        new_data = data->left(4);
        data_len =  ((0x00FF & new_data.at(3)) << 8) + (0xFF & new_data.at(2));


        //qDebug("packet length %d (%x, %x)", data_len, new_data.at(2), new_data.at(3));

        if ((unsigned char)new_data.at(1) == 0x10) {
            // Write BGR
            for (int j = 4; j < (4 + data_len); j += 3) {
                new_data += data->at(j+2);
                new_data += data->at(j+1);
                new_data += data->at(j);
            }
        } else if ((unsigned char)new_data.at(1) == 0x20) {
            // Write RGB
            new_data[1] = 0x10;
            for (int j = 4; j < (4 + data_len); j += 3) {
                new_data += data->at(j);
                new_data += data->at(j+1);
                new_data += data->at(j+2);
            }
        } else if ((unsigned char)new_data.at(1) == 0x30) {
            // Unpack HLS to RGB
            uint8_t r, g, b;
            new_data[1] = 0x10;

            // FIXME: hls_float_to_rgb_u8 should sanitize the HLS values first
            hls_float_to_rgb_u8(, &r, &g, &b);

            for (int j = 4; j < (4 + data_len); j += 3) {
                new_data += r;
                new_data += g;
                new_data += b;
            }
        } else if ((unsigned char)new_data.at(1) == 0x31) {
            // Unpack HLS to BGR
            uint8_t r, g, b;
            new_data[1] = 0x10;
            
            hls_float_to_rgb_u8(, &r, &g, &b);
            
            for (int j = 4; j < (4 + data_len); j += 3) {
                new_data += b;
                new_data += g;
                new_data += r;
            }
        }

        // Escape sequence
        escapes = 0;
        packet_size = new_data.size();
        for (int j = 2; j < packet_size; j++) {
            if (new_data.at(j) == (char)0x99) {
                new_data.insert(j + 1, (char)0x55);
                escapes++;
                packet_size++;
                //qDebug("ESCAPE %x %x", new_data.at(j), new_data.at(j+1));
                j++;
            }
        }

        // Fixup strand numbering
        new_data[0] = new_data.at(0) +  1;

        // Correct data length in case we added escape sequences
        new_data[2] = ((data_len + escapes) & 0xFF);
        new_data[3] = ((data_len + escapes) & 0xFF00) >> 8;


        // Start of frame sequence
        new_data.prepend((char)0x00);
        new_data.prepend((char)0x99);    

        //qDebug() << new_data.left(16).toHex();
        

        /*unsigned char checksum = 0;
        for (int i = 0; i < new_data.size(); i++) {
            //qDebug() << new_data.toHex().at(i);
            checksum ^= (unsigned char)new_data.at(i);
        }

        //qDebug() << "Checksum:" << checksum;
        
        

        new_data.append(checksum);*/

        //qDebug("Total length %d", data->length());

        //qDebug() << data->toHex();
        //if ((unsigned char)new_data.at(2) < 2)
        {
            emit data_ready(&new_data);
        }

        //qDebug("new_data %d, data %d", new_data.length(), data->length());

        data->remove(0, data_len + 4);

    } while (data->length() > 0);

    emit packet_done();
}

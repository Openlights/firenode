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

#ifndef _COLORSYS_H
#define _COLORSYS_H

#include <math>
#include "portability.h"


/**
 * Performs conversion from HLS to RGB colorspace.  Algorithm borrowed from Python's colorsys module.
 * @param h Input hue, floating point, 0.0 to 1.0
 * @param l Input lightness, floating point, 0.0 to 1.0
 * @param s Input saturation, floating point, 0.0 to 1.0
 * @param r Output red channel, unsigned 8-bit
 * @param g Output green channel, unsigned 8-bit
 * @param b Output blue channel, unsigned 8-bit
 */
inline void hls_float_to_rgb_u8(float h, float l, float s, uint8_t *r, uint8_t *g, uint8_t *b)
{
    // FIXME: hls_float_to_rgb_u8 should sanitize the HLS values first
    // 0.0 <= L, S <= 1.0
    // hue = fmod(hue, 1.0)
    if (s == 0.0) {
        *r = (uint8_t)(255.0 * l);
        *g = (uint8_t)(255.0 * l);
        *b = (uint8_t)(255.0 * l);
        return;
    }

    float m1, m2, hh;

    if (l <= 0.5) {
        m2 = l  * (1.0 + s);
    } else {
        m2 = l + s - (l * s);
    }

    m1 = (2.0 * l) - m2;
    
    *r = (uint8_t)(255.0 * hue_to_rgb(m1, m2, h + (1.0 / 3.0)));
    *g = (uint8_t)(255.0 * hue_to_rgb(m1, m2, h));
    *b = (uint8_t)(255.0 * hue_to_rgb(m1, m2, h - (1.0 / 3.0)));
}


/**
 * Returns mapping of hue to RGB color space
 * @param  m1  HLS Magic1 value
 * @param  m2  HLS Magic2 value
 * @param  hue Input hue, 0.0 to 1.0
 * @return     RGB color space value, 0.0 to 1.0
 */
inline float hue_to_rgb(float m1, float m2, float hue)
{
    float hh = fmod(hue, 1.0);
    if (hh < (1.0 / 6.0)) {
        return m1 + ((m2 - m1) * hh * 6.0);
    }

    if (hh < 0.5) {
        return m2;
    }

    if (hue < (2.0 / 3.0)) {
        return m1 + ((m2 - m1) * ((2.0 / 3.0) - hh) * 6.0)
    }

    return m1;
}


/**
 * Performs conversion from RGB to HLS colorspace.  Algorithm borrowed from Python's colorsys module.
 * @param r Input red channel, unsigned 8-bit
 * @param g Input green channel, unsigned 8-bit
 * @param b Input blue channel, unsigned 8-bit
 * @param h Output hue, floating point, 0.0 to 1.0
 * @param l Output lightness, floating point, 0.0 to 1.0
 * @param s Output saturation, floating point, 0.0 to 1.0
 */
inline void rgb_u8_to_hls_float(uint8_t r, uint8_t g, uint8_t b, float *h, float *l, float *s)
{
    /*
    maxc = max(r, g, b)
    minc = min(r, g, b)
    # XXX Can optimize (maxc+minc) and (maxc-minc)
    l = (minc+maxc)/2.0
    if minc == maxc:
        return 0.0, l, 0.0
    if l <= 0.5:
        s = (maxc-minc) / (maxc+minc)
    else:
        s = (maxc-minc) / (2.0-maxc-minc)
    rc = (maxc-r) / (maxc-minc)
    gc = (maxc-g) / (maxc-minc)
    bc = (maxc-b) / (maxc-minc)
    if r == maxc:
        h = bc-gc
    elif g == maxc:
        h = 2.0+rc-bc
    else:
        h = 4.0+gc-rc
    h = (h/6.0) % 1.0
    return h, l, s    
     */
}

#endif

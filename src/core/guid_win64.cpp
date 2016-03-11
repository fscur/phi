/*
The MIT License (MIT)

Copyright (c) 2014 Graeme Hill (http://graemehill.ca)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <precompiled.h>

#include "guid.h"

#include <objbase.h>

guid guidGenerator::newGuid()
{
    GUID newId;
    CoCreateGuid(&newId);

    const unsigned char bytes[16] =
    {
        (newId.Data1 >> 24) & 0xFF,
        (newId.Data1 >> 16) & 0xFF,
        (newId.Data1 >> 8) & 0xFF,
        (newId.Data1) & 0xff,

        (newId.Data2 >> 8) & 0xFF,
        (newId.Data2) & 0xff,

        (newId.Data3 >> 8) & 0xFF,
        (newId.Data3) & 0xFF,

        newId.Data4[0],
        newId.Data4[1],
        newId.Data4[2],
        newId.Data4[3],
        newId.Data4[4],
        newId.Data4[5],
        newId.Data4[6],
        newId.Data4[7]
    };

    return bytes;
}
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

#include <core/base64.h>

#include "guid.h"

using namespace std;

namespace phi
{
    guid::guid(const vector<byte>& bytes) :
        _bytes(bytes)
    {
    }

    guid::guid(const byte* bytes)
    {
        _bytes.assign(bytes, bytes + 16);
    }

    byte hexDigitToChar(char ch)
    {
        if (ch > 47 && ch < 58)
            return ch - 48;

        if (ch > 96 && ch < 103)
            return ch - 87;

        if (ch > 64 && ch < 71)
            return ch - 55;

        return 0;
    }

    byte hexPairToChar(char a, char b)
    {
        return hexDigitToChar(a) * 16 + hexDigitToChar(b);
    }

    guid::guid(const string& fromHexaString)
    {
        _bytes.clear();

        char charOne = 0;
        char charTwo = 0;

        bool lookingForFirstChar = true;

        for (const char &ch : fromHexaString)
        {
            if (ch == '-')
                continue;

            if (lookingForFirstChar)
            {
                charOne = ch;
                lookingForFirstChar = false;
            }
            else
            {
                charTwo = ch;
                auto byte = hexPairToChar(charOne, charTwo);
                _bytes.push_back(byte);
                lookingForFirstChar = true;
            }
        }
    }

    guid::guid() :
        _bytes(std::vector<byte>(16, 0))
    {
    }

    guid::guid(const guid& other) :
        _bytes(other._bytes)
    {
    }

    guid& guid::operator=(const guid& other)
    {
        if (&other != this)
            _bytes = other._bytes;

        return *this;
    }

    bool guid::operator==(const guid& other) const
    {
        return _bytes == other._bytes;
    }

    bool guid::operator!=(const guid& other) const
    {
        return !((*this) == other);
    }

    ostream& operator<<(ostream& s, const guid& guid)
    {
        return s << hex << setfill('0')
            << setw(2) << (int)guid._bytes[0]
            << setw(2) << (int)guid._bytes[1]
            << setw(2) << (int)guid._bytes[2]
            << setw(2) << (int)guid._bytes[3]
            << "-"
            << setw(2) << (int)guid._bytes[4]
            << setw(2) << (int)guid._bytes[5]
            << "-"
            << setw(2) << (int)guid._bytes[6]
            << setw(2) << (int)guid._bytes[7]
            << "-"
            << setw(2) << (int)guid._bytes[8]
            << setw(2) << (int)guid._bytes[9]
            << "-"
            << setw(2) << (int)guid._bytes[10]
            << setw(2) << (int)guid._bytes[11]
            << setw(2) << (int)guid._bytes[12]
            << setw(2) << (int)guid._bytes[13]
            << setw(2) << (int)guid._bytes[14]
            << setw(2) << (int)guid._bytes[15];
    }

    string guid::toStringBase64() const
    {
        return base64::encode(&_bytes[0], _bytes.size());
    }

    vector<byte>& guid::getBytes()
    {
        return _bytes;
    }
}
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

#pragma once

#include <phi.h>
#include "coreApi.h"

namespace phi
{
    class guid
    {
    private:
        vector<byte> _bytes;

    private:
        CORE_API friend std::ostream &operator<<(std::ostream& s, const guid& guid);

    public:
        CORE_API guid(const vector<byte>& bytes);
        CORE_API guid(const byte* bytes);
        CORE_API guid(const string& fromString);
        CORE_API guid();
        CORE_API guid(const guid& other);
        CORE_API guid& operator=(const guid& other);
        CORE_API bool operator==(const guid& other) const;
        CORE_API bool operator!=(const guid& other) const;
        CORE_API string toStringBase64() const;
        CORE_API vector<byte>& getBytes();

    public:
        CORE_API static guid newGuid();


    };
}

namespace std
{
    //http://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key
    template <>
    struct hash<phi::guid>
    {
        std::size_t operator()(phi::guid& guid) const
        {
            using std::size_t;
            using std::hash;

            std::size_t seed = 0;

            for (auto& item : guid.getBytes())
            {
                std::hash_combine<GLenum>(seed, item);
            }

            return seed;
        }
    };
}
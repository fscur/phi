#include "base64.h"

#include <iostream>

namespace phi
{
    static const std::string base64Chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

    static inline bool isBase64(byte c)
    {
        return (isalnum(c) || (c == '+') || (c == '/'));
    }

    std::string base64::encode(byte const* buf, unsigned int bufLen)
    {
        std::string ret;
        int i = 0;
        int j = 0;
        byte charArray3[3];
        byte charArray4[4];

        while (bufLen--)
        {
            charArray3[i++] = *(buf++);
            if (i == 3)
            {
                charArray4[0] = (charArray3[0] & 0xfc) >> 2;
                charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
                charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
                charArray4[3] = charArray3[2] & 0x3f;

                for (i = 0; (i < 4); i++)
                    ret += base64Chars[charArray4[i]];
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j < 3; j++)
                charArray3[j] = '\0';

            charArray4[0] = (charArray3[0] & 0xfc) >> 2;
            charArray4[1] = ((charArray3[0] & 0x03) << 4) + ((charArray3[1] & 0xf0) >> 4);
            charArray4[2] = ((charArray3[1] & 0x0f) << 2) + ((charArray3[2] & 0xc0) >> 6);
            charArray4[3] = charArray3[2] & 0x3f;

            for (j = 0; (j < i + 1); j++)
                ret += base64Chars[charArray4[j]];

            while ((i++ < 3))
                ret += '=';
        }

        return ret;
    }

    std::vector<byte> base64::decode(std::string const& encodedString)
    {
        int in_len = (int)encodedString.size();
        int i = 0;
        int j = 0;
        int in_ = 0;
        byte charArray4[4], charArray3[3];
        std::vector<byte> ret;

        while (in_len-- && (encodedString[in_] != '=') && isBase64(encodedString[in_]))
        {
            charArray4[i++] = encodedString[in_]; in_++;
            if (i == 4)
            {
                for (i = 0; i <4; i++)
                    charArray4[i] = (byte)base64Chars.find(charArray4[i]);

                charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
                charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
                charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

                for (i = 0; (i < 3); i++)
                    ret.push_back(charArray3[i]);
                i = 0;
            }
        }

        if (i)
        {
            for (j = i; j <4; j++)
                charArray4[j] = 0;

            for (j = 0; j <4; j++)
                charArray4[j] = (byte)base64Chars.find(charArray4[j]);

            charArray3[0] = (charArray4[0] << 2) + ((charArray4[1] & 0x30) >> 4);
            charArray3[1] = ((charArray4[1] & 0xf) << 4) + ((charArray4[2] & 0x3c) >> 2);
            charArray3[2] = ((charArray4[2] & 0x3) << 6) + charArray4[3];

            for (j = 0; (j < i - 1); j++) ret.push_back(charArray3[j]);
        }

        return ret;
    }
}
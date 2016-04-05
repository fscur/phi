#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\resource.h>
#include "texture.h"

namespace phi
{
    class RENDERING_API font
    {
    private:
        static const int MAX_WIDTH = 1024;

        int _size;
        int _texWidth;
        int _texHeight;
        int _baseLine;
        int _ascender;
        int _lineHeight;
        texture* _texture;

    public:
        struct
        {
            float ax; // advance.x
            float ay; // advance.y

            float bw; // bitmap.width
            float bh; // bitmap.height

            float bl; // bitmap_left
            float bt; // bitmap_top (distance from baseline to the top of the glyph)

            float tx; // x offset of glyph in texture coordinates
            float ty; // y offset of glyph in texture coordinates
        } c[128];

    public:
        font(std::string name, std::string path, int size, FT_Library library);
        ~font();

        static font* fromFile(string fileName);//TODO: unimplemented method

        int getTexWidth() { return _texWidth; }
        int getTexHeight() { return _texHeight; }
        int getBaseLine() { return _baseLine; }
        int getAscender() { return _ascender; }
        int getLineHeight() { return _lineHeight; }
        texture* getTexture() { return _texture; }
    };
}
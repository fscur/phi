#include "font.h"

#include <core/resource.h>

#include <algorithm>

namespace phi
{
    font::font(std::string name, std::string path, int size, FT_Library library)
    {
        _size = size;

        // TODO: all this code should be on the importer class?     Yes it totally should, good luck!!!!
        FT_Face face;
        if (FT_New_Face(library, path.c_str(), 0, &face))
            log("Could not open font");

        FT_Set_Pixel_Sizes(face, 0, _size);
        FT_GlyphSlot g = face->glyph;

        int roww = 0;
        int rowh = 0;
        _texWidth = 0;
        _texHeight = 0;

        memset(c, 0, sizeof c);

         //Find minimum size for a texture holding all visible ASCII characters
        for (int i = 32; i < 256; i++)
        {
            if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            {
                fprintf(stderr, "Loading character %c failed!\n", i);
                continue;
            }

            if (roww + g->bitmap.width + 1 >= MAX_WIDTH)
            {
                _texWidth = std::max(_texWidth, roww);
                _texHeight += rowh;
                roww = 0;
                rowh = 0;
            }
            roww += g->bitmap.width + 1;
            rowh = std::max(rowh, (int)g->bitmap.rows);
        }

        _texWidth = std::max(_texWidth, roww);
        _texHeight += rowh;

        //_texture = texture::create(_texWidth, _texHeight, GL_RGBA, GL_RED, GL_UNSIGNED_BYTE);
        _texture = new texture(_texWidth, _texHeight, GL_RGBA);
        //_texture->bind(0);

        // We require 1 byte alignment when uploading texture data
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Clamping to edges is important to prevent artifacts when scaling
        //_texture->setParam(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //_texture->setParam(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // Linear filtering usually looks best for text
        //_texture->setParam(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //_texture->setParam(GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Paste all glyph bitmaps into the texture, remembering the offset
        int ox = 0;
        int oy = 0;

        rowh = 0;
        for (int i = 32; i < 128; i++)
        {
            if (FT_Load_Char(face, i, FT_LOAD_RENDER))
            {
                fprintf(stderr, "Loading character %c failed!\n", i);
                continue;
            }

            if (ox + g->bitmap.width + 1 >= MAX_WIDTH)
            {
                oy += rowh;
                rowh = 0;
                ox = 0;
            }

            glTexSubImage2D(GL_TEXTURE_2D, 0, ox, oy, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
            c[i].ax = (float)(g->advance.x >> 6);
            c[i].ay = (float)(g->advance.y >> 6);

            c[i].bw = (float)g->bitmap.width;
            c[i].bh = (float)g->bitmap.rows;

            c[i].bl = (float)g->bitmap_left;
            c[i].bt = (float)g->bitmap_top;

            c[i].tx = ox / (float)_texWidth;
            c[i].ty = oy / (float)_texHeight;

            rowh = std::max(rowh, (int)g->bitmap.rows);
            ox += g->bitmap.width + 1;
        }

        _baseLine = (int)abs(face->size->metrics.descender / 64.0f);
        _ascender = (int)abs(face->size->metrics.ascender / 64.0f);
        _lineHeight = (int)(face->size->metrics.height / 64.0f);
    }

    font::~font()
    {
        safeDelete(_texture);
    }
}
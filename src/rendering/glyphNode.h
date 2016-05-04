#pragma once
#include "renderingApi.h"
#include "glyph.h"
#include "rectangle.h"

namespace phi
{
    class glyphNode
    {
    private:
        glyphNode* _left;
        glyphNode* _right;
        glyph* _glyph;

    public:
        rectangle rect;

    private:
        bool isLeaf();
        glyphNode* insert(glyph* glyph, glyphNode* node);

    public:
        RENDERING_API glyphNode(rectangle rect);
        RENDERING_API ~glyphNode();

        RENDERING_API glyphNode* insert(glyph* glyph);
    };
}
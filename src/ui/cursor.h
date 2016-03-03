#pragma once
#include <phi.h>

#include <core\resource.h>

#include <rendering/texture.h>

#include "ui.h"

namespace phi
{
    class cursor
    {
    private:
        texture* _texture;
        vec2 _hotPoint;

    public:
        cursor(std::string path, vec2 hotPoint);
        ~cursor();

        texture* getTexture() { return _texture; }
        vec2 getHotPoint() { return _hotPoint; }
    };
}
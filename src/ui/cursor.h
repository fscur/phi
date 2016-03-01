#ifndef _PHI_CURSOR_H_
#define _PHI_CURSOR_H_

#include <core\globals.h>
#include <core\resource.h>

#include <rendering/texture.h>

#include "ui.h"

#include <string>

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

#endif
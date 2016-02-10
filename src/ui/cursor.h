#ifndef _PHI_CURSOR_H_
#define _PHI_CURSOR_H_

#include <phi/core/globals.h>
#include <phi/core/resource.h>

#include <phi/rendering/texture.h>

#include "ui.h"

#include <string>

namespace phi
{
    class cursor : 
        public resource
    {
    private:
        texture* _texture;
        vec2 _hotPoint;

    public:
        cursor(std::string name, std::string path, vec2 hotPoint);
        ~cursor();

        texture* getTexture() { return _texture; }
        vec2 getHotPoint() { return _hotPoint; }
    };
}

#endif
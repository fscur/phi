#ifndef _PHI_CURSOR_H_
#define _PHI_CURSOR_H_

#include "ui.h"
#include <string>
#include "resource.h"
#include "texture.h"
#include <glm\glm.hpp>

namespace phi
{
    class cursor : 
        public resource
    {
    private:
        texture* _texture;
        glm::vec2 _hotPoint;

    public:
        cursor(std::string name, std::string path, glm::vec2 hotPoint);
        ~cursor();

        texture* getTexture() { return _texture; }
        glm::vec2 getHotPoint() { return _hotPoint; }
    };
}

#endif
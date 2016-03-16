#pragma once
#include <phi.h>
#include "textureContainer.h"
#include "glError.h"

namespace phi
{
    class texturesManager
    {
    private:
        const size_t MAX_CONTAINER_ITEMS = 10;
        std::map<
            std::tuple<GLsizei, GLsizei, GLsizei, GLenum, GLint, GLint, GLint>, 
            vector<textureContainer*>> _containers;

        bool _bindless;
        bool _sparse;

        GLint _currentTextureUnit;
        GLint _maxTextureUnits;
        size_t _maxContainerSize;

    public:
        vector<GLint> units;
        vector<GLuint64> handles;

    public:
        texturesManager(bool bindless, bool sparse);
        ~texturesManager();

        phi::textureAddress add(phi::texture* texture);
        void reserveContainer(textureContainerLayout layout, size_t size);
    };
}
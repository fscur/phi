#pragma once

#include "textureContainer.h"

namespace phi
{
    class texturesManager
    {
    private:
        const size_t MAX_CONTAINER_ITEMS = 10;
        std::map<
            std::tuple<GLsizei, GLsizei, GLsizei, GLenum, GLint, GLint, GLint>, 
            std::vector<textureContainer*>> _containers;

        bool _bindless;
        bool _sparse;

        GLint _currentTextureUnit;
        GLint _maxTextureUnits;
        size_t _maxContainerSize;

    public:
        std::vector<GLint> units;
        std::vector<GLuint64> handles;

    public:
        texturesManager(bool bindless = false, bool sparse = false) :
            _bindless(bindless),
            _sparse(sparse),
            _currentTextureUnit(-1),
            _maxContainerSize(MAX_CONTAINER_ITEMS)
        {
            //TODO: calcular quanto de memoria tem disponivel na GPU
            //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
            //TODO: controlar a memoria da gpu usada

            if (_sparse)
            {
                GLint maxContainerSize;
                glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_ARB, &maxContainerSize);
                _maxContainerSize = static_cast<size_t>(maxContainerSize);
            }

            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &_maxTextureUnits);
        }

        ~texturesManager()
        {
            for (auto &pair : _containers)
            {
                auto containers = pair.second;
                auto containersCount = containers.size();
                for (uint i = 0; i < containersCount; ++i)
                {
                    delete containers[i];
                }
            }
        }

        phi::textureAddress add(phi::texture* texture);
        void reserveContainer(textureContainerLayout layout, size_t size);
    };
}
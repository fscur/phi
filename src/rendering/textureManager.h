#pragma once

#include "textureContainer.h"
#include "gl.h"

namespace phi
{
    class textureManager
    {
    private:
        const uint MAX_CONTAINER_ITEMS = 10u;
        std::map<std::tuple<GLsizei, GLsizei, GLsizei, GLenum>, std::vector<textureContainer*>> _containers;

        bool _bindless;
        bool _sparse;

        GLint _currentTextureUnit;
        GLint _maxTextureUnits;
        GLint _maxContainerItems;

    public:
        std::vector<GLint> units;
        std::vector<GLuint64> handles;

    public:
        textureManager() :
            _bindless(false),
            _sparse(false),
            _currentTextureUnit(-1),
            _maxContainerItems(MAX_CONTAINER_ITEMS)
        {
            //TODO: calcular quanto de memoria tem disponivel na GPU
            //TODO: verificar quando de memoria nosso gbuffer + shadow maps usam e ver quanto sobra pra texturas
            //TODO: controlar a memoria da gpu usada
            
            _bindless = phi::gl::currentState->useBindlessTextures;
            _sparse = phi::gl::currentState->useSparseTextures;

            if (_sparse)
                glGetIntegerv(GL_MAX_SPARSE_ARRAY_TEXTURE_LAYERS_ARB, &_maxContainerItems);

            glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS_ARB, &_maxTextureUnits);
        }

        ~textureManager()
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

        textureAddress add(texture* tex);
    };
}
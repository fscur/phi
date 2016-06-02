#pragma once
#include <phi.h>
#include <core\rectangle.h>

#include "texture.h"
#include "textureAddress.h"
#include "textureContainerLayout.h"

namespace phi
{
    class textureContainer
    {
    private:
        size_t _freeSpace;
        bool _created;

    protected:
        size_t _maxTextures;
        textureContainerLayout _layout;
        GLint _unit;

    public:
        GLuint id;
        GLuint64 handle;
        map<const texture*, phi::textureAddress> texturesAddresses;
        vector<const texture*> textures;
    
    private:
        void create();
        void loadTexture(const texture* const texture);

    protected:
        virtual void onCreate();
        virtual void onLoadTexture(const texture* const texture);
        virtual void onSubData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data);

    public:
        textureContainer(
            textureContainerLayout layout,
            size_t maxTextures,
            GLint unit);

        virtual ~textureContainer();

        bool add(const texture* const texture, textureAddress& textureAddress);
        void subData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data);
    };
}

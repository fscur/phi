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
        GLuint _id;
        GLuint64 _handle;
        vector<const texture*> _textures;
        map<const texture*, phi::textureAddress> _texturesAddresses;

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
            size_t maxTextures);

        virtual ~textureContainer();

        bool add(const texture* const texture, textureAddress& textureAddress);
        void subData(
            const float& page,
            const rectangle<GLint>& rect,
            const void* const data);

        GLint getUnit() const { return _unit; }
        GLuint64 getHandle() const { return _handle; }
    };
}

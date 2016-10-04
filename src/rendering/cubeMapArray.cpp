#include <precompiled.h>
#include "cubeMapArray.h"
#include "textureUnits.h"

namespace phi
{
    cubeMapArray::cubeMapArray(
        sizeui size,
        textureLayout layout) :
        _created(false),
        _pages(unordered_map<float, bool>()),
        _size(size),
        _maxPages(size.d),
        _layout(layout),
        _unit(-1),
        _id(0),
        _handle(0),
        _cubeMaps(vector<const cubeMap*>()),
        _cubeMapsAddresses(map<const cubeMap*, phi::textureAddress>())
    {
        glCreateTextures(GL_TEXTURE_CUBE_MAP_ARRAY, 1, &_id);

        _unit = textureUnits::get();

        for (auto i = 0u; i < _maxPages; ++i)
            _pages[static_cast<float>(i)] = false;
    }

    cubeMapArray::~cubeMapArray()
    {
        glDeleteTextures(1, &_id);

        textureUnits::clear(_unit);

        for (auto& cubeMap : _cubeMaps)
            safeDelete(cubeMap);
    }

    void cubeMapArray::create()
    {
        onCreate();

        _created = true;
    }

    float cubeMapArray::getAvailablePage()
    {
        for (auto i = 0u; i < _maxPages; ++i)
        {
            auto key = static_cast<float>(i);
            bool pageNotUsed = !_pages[key];
            
            if (pageNotUsed)
                return key;
        }

        return -1.0f;
    }

    bool cubeMapArray::loadData(
        const cubeMap* cubeMap,
        textureAddress& cubeMapAddress)
    {
        float page = getAvailablePage();

        if (page < 0.0f)
            return false;

        cubeMapAddress.arrayId = _id;
        cubeMapAddress.unit = _unit;
        cubeMapAddress.page = page;

        _cubeMaps.push_back(cubeMap);
        _cubeMapsAddresses[cubeMap] = cubeMapAddress;

        loadData(page, cubeMap);

        return true;
    }

    void cubeMapArray::loadData(
        float page,
        const cubeMap* cubeMap)
    {
        onLoadData(page, cubeMap);
    }

    void cubeMapArray::onCreate()
    {
        glActiveTexture(GL_TEXTURE0 + _unit);

        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _id);

        glTextureStorage3D(_id,
            _layout.levels,
            _layout.internalFormat,
            _size.w,
            _size.h,
            static_cast<GLsizei>(_maxPages * 6));


        glTextureParameteri(_id, GL_TEXTURE_WRAP_S, _layout.wrapMode);
        glTextureParameteri(_id, GL_TEXTURE_WRAP_T, _layout.wrapMode);
        glTextureParameteri(_id, GL_TEXTURE_WRAP_R, _layout.wrapMode);
        glTextureParameteri(_id, GL_TEXTURE_MIN_FILTER, _layout.minFilter);
        glTextureParameteri(_id, GL_TEXTURE_MAG_FILTER, _layout.magFilter);
    }

    void cubeMapArray::onLoadData(
        float page,
        const cubeMap* cubeMap)
    {
        glActiveTexture(GL_TEXTURE0 + _unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, _id);
        
        for (size_t i = 0; i < 6; i++)
        {
            glTextureSubImage3D(
                _id,
                0,
                0,
                0,
                static_cast<GLint>(page + i),
                _size.w,
                _size.h,
                1,
                _layout.dataFormat,
                _layout.dataType,
                cubeMap->data[i]);
        }

        glGenerateMipmap(GL_TEXTURE_CUBE_MAP_ARRAY);
    }

    bool cubeMapArray::add(
        const cubeMap* const cubeMap,
        textureAddress& cubeMapAddress)
    {
        if (phi::contains(_cubeMaps, cubeMap))
        {
            cubeMapAddress = _cubeMapsAddresses[cubeMap];
            return true;
        }

        if (!_created)
            create();

        return loadData(cubeMap, cubeMapAddress);
    }

    void cubeMapArray::remove(const cubeMap* cubeMap)
    {
        auto address = _cubeMapsAddresses[cubeMap];

        phi::removeIfContains(_cubeMaps, cubeMap);
        _cubeMapsAddresses.erase(cubeMap);

        _pages[address.page] = false;

        safeDelete(cubeMap);

        //TODO:remove sparse page
    }

    bool cubeMapArray::isEmpty()
    {
        return _cubeMaps.size() == 0;
    }

    void cubeMapArray::release()
    {
        glDeleteTextures(1, &_id);
        textureUnits::clear(_unit);
    }
}
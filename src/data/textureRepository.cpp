#include <precompiled.h>

#include <io/path.h>

#include "textureRepository.h"
#include "importResourceException.h"
#include "model/textureDTO.h"
#include "utils/cerealSerializationFunctions.h"

namespace phi
{
    using namespace cereal;

    textureRepository::textureRepository(string libraryPath) :
        _libraryPath(libraryPath)
    {
        auto indexPath = path::combine(_libraryPath, TEXTURES_INDEX_FILE_NAME);
        _index = index::load(indexPath);
    }

    textureRepository::~textureRepository()
    {
    }

    image* textureRepository::getById(guid guid)
    {
        if (phi::contains(_cache, guid))
            return _cache[guid];

        for (auto& item : _cache)
            if (item.first == guid)
            {
                auto t = item.second;
                return t;
            }

        if (!_index.contains(guid))
            return nullptr;

        auto entry = _index.getEntryById(guid);
        auto texturePath = path::combine(_libraryPath, entry.path);
        auto image = loadImage(texturePath, guid);

        _cache[guid] = image;
        return image;

        //std::ifstream inputFileStream(texturePath);
        //JSONInputArchive archive(inputFileStream);

        //textureDTO textureDTO;
        //textureDTO.serialize(archive);

        //auto textureDirectory = path::getDirectoryFullName(texturePath);

        //auto filePath = path::combine(textureDirectory, textureDTO.imageFileName);
    }

    image* textureRepository::loadImage(string filePath, guid guid)
    {
        auto fileNameChar = filePath.c_str();
        auto imageFormat = FreeImage_GetFileType(fileNameChar, 0);

        if (imageFormat == FIF_UNKNOWN)
            imageFormat = FreeImage_GetFIFFromFilename(fileNameChar);

        if (imageFormat == FIF_UNKNOWN)
            throw importResourceException("Format not identified importing texture", filePath);

        if (!FreeImage_FIFSupportsReading(imageFormat))
            throw importResourceException("Image format not supported ", filePath);

        auto imagePointer = FreeImage_Load(imageFormat, fileNameChar);
        if (!imagePointer)
            throw importResourceException("Failed loading image", filePath);

        auto dataPtr = FreeImage_GetBits(imagePointer);
        auto width = FreeImage_GetWidth(imagePointer);
        auto height = FreeImage_GetHeight(imagePointer);

        if (dataPtr == nullptr || width == 0 || height == 0)
            throw importResourceException("Failed loading image data of", filePath);

        auto format = imageDataFormat::bgra;
        auto bpp = FreeImage_GetBPP(imagePointer);
        auto redMask = FreeImage_GetRedMask(imagePointer);

        switch (bpp)
        {
        case 24:
            if (redMask == 255)
                format = imageDataFormat::rgb;
            else
                format = imageDataFormat::bgr;
            break;
        case 32:
            if (redMask == 255)
                format = imageDataFormat::rgba;
            else
                format = imageDataFormat::bgra;
            break;
        }

        auto bytesPerPixel = bpp / 8;
        auto totalBytes = width * height * bytesPerPixel;
        auto data = new byte[totalBytes];
        memcpy(data, dataPtr, totalBytes);

        FreeImage_Unload(imagePointer);

        //TODO: does all images need a guid?
        return new image(
            guid,
            width,
            height,
            format,
            imageDataType::ubyte_dataType,
            data);
    }
}
#include <precompiled.h>
#include "modelRepository.h"

#include <io/path.h>
#include <io/fileReader.h>

#include <strstream>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

namespace phi
{
    modelRepository::modelRepository(string libraryPath) :
        _libraryPath(libraryPath)
    {
        loadIndex();
    }

    modelRepository::~modelRepository()
    {
    }

    void modelRepository::loadIndex()
    {
        string indexPath = path::combine(_libraryPath, "models.index");
        std::ifstream inputFileStream(indexPath);
        cereal::JSONInputArchive archive(inputFileStream);

        _index.load(archive);
    }

    modelJson modelRepository::getModelByIndex(int index)
    {
        _unused(index);
        return modelJson();
    }
}
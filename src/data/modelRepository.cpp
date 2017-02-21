#include <precompiled.h>
#include "modelRepository.h"

#include <io/path.h>
#include <io/fileReader.h>

#include "utils/cerealSerializationFunctions.h"

namespace phi
{
    using namespace cereal;

    modelRepository::modelRepository(string libraryPath) :
        _libraryPath(libraryPath)
    {
        auto indexPath = path::combine(_libraryPath, MODELS_INDEX_FILE_NAME);
        _index = index::load(indexPath);
    }

    modelRepository::~modelRepository()
    {
    }

    modelDTO modelRepository::getModelByIndex(int index)
    {
        auto entry = _index.getEntryByIndex(index);
        auto modelPath = path::combine(_libraryPath, entry.path);
        std::ifstream inputFileStream(modelPath);
        JSONInputArchive archive(inputFileStream);

        auto model = modelDTO();
        model.serialize(archive);

        return model;
    }

    modelDTO modelRepository::getModelById(const phi::guid& id)
    {
        auto entry = _index.getEntryById(id);
        auto modelPath = path::combine(_libraryPath, entry.path);
        std::ifstream inputFileStream(modelPath);
        JSONInputArchive archive(inputFileStream);

        auto model = modelDTO();
        model.serialize(archive);

        return model;
    }
}
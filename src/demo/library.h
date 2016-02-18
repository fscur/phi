#pragma once

#include <core\resourcesRepository.h>
#include <core\object3D.h>
#include <rendering\material.h>
#include <functional>

class library
{
private:
    std::string _libraryPath;
    phi::resourcesRepository<phi::texture>* _texturesRepository;
    phi::resourcesRepository<phi::material>* _materialsRepository;
    phi::resourcesRepository<phi::object3D>* _objectsRepository;

private:

    template<typename T>
    phi::resourcesRepository<T>* load(
        std::string directory, 
        std::string filter, 
        std::function<bool(std::string, phi::resource<T>*&)> importFunction)
    {
        auto repository = new phi::resourcesRepository<T>();
        auto filters = std::vector<std::string>();
        filters.push_back(filter);

        load(directory, filters, repository, importFunction);

        return repository;
    }

    template<typename T>
    void load(std::string directory, 
        std::vector<std::string> filters, 
        phi::resourcesRepository<T>* repository, 
        std::function<bool(std::string, phi::resource<T>*&)> importFunction)
    {
        auto subDirs = phi::path::getDirectories(directory);
        for (auto &dir : subDirs)
            load(dir.path, filters, repository, importFunction);

        auto files = phi::path::getFiles(directory, filters);
        for (auto &file : files)
        {
            phi::resource<T>* resource;
            if (importFunction(file.path, resource))
                repository->addResource(resource);
        }
    }

public:
    library(std::string resourcesPath);
    void init();

    phi::resourcesRepository<phi::texture>* getTexturesRepository() const { return _texturesRepository; }
    phi::resourcesRepository<phi::material>* getMaterialsRepository() const { return _materialsRepository; }
    phi::resourcesRepository<phi::object3D>* getObjectsRepository() const { return _objectsRepository; }
};
#pragma once
#include <phi.h>
#include <core\resourcesRepository.h>
#include <core\node.h>
#include <rendering\material.h>
#include <rendering\gl.h>


class library
{
private:
    phi::gl* _gl;
    phi::string _libraryPath;
    phi::resourcesRepository<phi::texture>* _texturesRepository;
    phi::resourcesRepository<phi::material>* _materialsRepository;
    phi::resourcesRepository<phi::node>* _nodesRepository;

private:

    template<typename T>
    phi::resourcesRepository<T>* load(
        phi::string directory, 
        phi::string filter, 
        std::function<bool(phi::string, phi::resource<T>*&)> importFunction)
    {
        auto repository = new phi::resourcesRepository<T>();
        auto filters = phi::vector<phi::string>();
        filters.push_back(filter);

        load(directory, filters, repository, importFunction);

        return repository;
    }

    template<typename T>
    void load(phi::string directory, 
        phi::vector<phi::string> filters, 
        phi::resourcesRepository<T>* repository, 
        std::function<bool(phi::string, phi::resource<T>*&)> importFunction)
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

    library(phi::gl* gl, phi::string resourcesPath);

    void init();

    phi::resourcesRepository<phi::texture>* getTexturesRepository() const { return _texturesRepository; }
    phi::resourcesRepository<phi::material>* getMaterialsRepository() const { return _materialsRepository; }
    phi::resourcesRepository<phi::node>* getObjectsRepository() const { return _nodesRepository; }
};
#pragma once
#include <phi.h>

#include <core\resourcesRepository.h>
#include <core\node.h>
#include <core\geometry.h>

#include <io\path.h>

#include <loader\importResourceException.h>

#include <rendering\material.h>
#include <rendering\gl.h>

#include <apps\application.h>

namespace demon
{
    class library
    {
    private:
        phi::gl* _gl;
        phi::string _libraryPath;
        phi::resourcesRepository<phi::texture>* _texturesRepository;
        phi::resourcesRepository<phi::geometry>* _geometriesRepository;
        phi::resourcesRepository<phi::material>* _materialsRepository;
        phi::resourcesRepository<phi::node>* _nodesRepository;

    private:

        template<typename T>
        phi::resourcesRepository<T>* load(
            phi::string directory,
            phi::string filter,
            std::function<phi::resource<T>*(phi::string)> importFunction)
        {
            auto repository = new phi::resourcesRepository<T>();

            load(directory, { filter }, repository, importFunction);

            return repository;
        }

        template<typename T>
        void load(phi::string directory,
            phi::vector<phi::string> filters,
            phi::resourcesRepository<T>* repository,
            std::function<phi::resource<T>*(phi::string)> importFunction)
        {
            auto subDirs = phi::path::getDirectories(directory);
            for (phi::directoryInfo &dir : subDirs)
                load(dir.path, filters, repository, importFunction);

            auto files = phi::path::getFiles(directory, filters);

            for (auto &file : files)
            {
                try
                {
                    repository->addResource(importFunction(file.path));
                }
                catch (const phi::importResourceException& exception)
                {
                    phi::application::logError(phi::string(exception.what()) + " " + exception.getResourcePath());
                }
            }
        }

    public:

        library(phi::gl* gl, phi::string resourcesPath);

        void init();

        phi::resourcesRepository<phi::texture>* getTexturesRepository() const { return _texturesRepository; }
        phi::resourcesRepository<phi::geometry>* getGeometriesRepository() const { return _geometriesRepository; }
        phi::resourcesRepository<phi::material>* getMaterialsRepository() const { return _materialsRepository; }
        phi::resourcesRepository<phi::node>* getObjectsRepository() const { return _nodesRepository; }
    };
}
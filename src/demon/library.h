#pragma once
#include <phi.h>

#include <core\resourcesRepository.h>
#include <core\node.h>
#include <core\geometry.h>
#include <core\material.h>

#include <io\path.h>
#include <loader\importResourceException.h>
#include <application\application.h>

namespace demon
{
    class library
    {
    private:
        phi::string _libraryPath;
        phi::resourcesRepository<phi::image>* _imagesRepository;
        phi::resourcesRepository<phi::material>* _materialsRepository;
        phi::resourcesRepository<phi::geometry>* _geometriesRepository;
        phi::resourcesRepository<phi::node>* _nodesRepository;

    private:
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

        library(phi::string resourcesPath);
        ~library();

        void load();

        phi::resourcesRepository<phi::image>* getTexturesRepository() const { return _imagesRepository; }
        phi::resourcesRepository<phi::geometry>* getGeometriesRepository() const { return _geometriesRepository; }
        phi::resourcesRepository<phi::material>* getMaterialsRepository() const { return _materialsRepository; }
        phi::resourcesRepository<phi::node>* getObjectsRepository() const { return _nodesRepository; }
    };
}
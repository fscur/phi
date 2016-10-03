#pragma once
#include <phi.h>

#include <core/entityRepository.h>
#include <core/model.h>
#include <core/image.h>
#include <core/geometry.h>
#include <core/material.h>

#include <io/path.h>
#include <loader/importResourceException.h>
#include <application/application.h>

namespace demon
{
    class library
    {
    private:
        phi::string _libraryPath;
        phi::entityRepository<phi::image>* _imagesRepository;
        phi::entityRepository<phi::material>* _materialsRepository;
        phi::entityRepository<phi::geometry>* _geometriesRepository;
        phi::entityRepository<phi::model>* _modelsRepository;

    private:
        template<typename T>
        void load(phi::string directory,
            phi::vector<phi::string> filters,
            phi::entityRepository<T>* repository,
            std::function<phi::entity*(phi::string)> importFunction)
        {
            auto subDirs = phi::path::getDirectories(directory);
            for (phi::directoryInfo &dir : subDirs)
                load(dir.path, filters, repository, importFunction);

            auto files = phi::path::getFiles(directory, filters);

            for (auto& file : files)
            {
                try
                {
                    repository->add((T*)importFunction(file.path));
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

        phi::entityRepository<phi::image>* getTexturesRepository() const { return _imagesRepository; }
        phi::entityRepository<phi::geometry>* getGeometriesRepository() const { return _geometriesRepository; }
        phi::entityRepository<phi::material>* getMaterialsRepository() const { return _materialsRepository; }
        phi::entityRepository<phi::model>* getObjectsRepository() const { return _modelsRepository; }
    };
}
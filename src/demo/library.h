#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include <core\resourcesRepository.h>
#include <core\object3D.h>
#include <rendering\material.h>

class library
{
private:
    std::string _resourcesPath;
    phi::resourcesRepository<phi::texture>* _texturesRepository;
    phi::resourcesRepository<phi::material>* _materialsRepository;
    phi::resourcesRepository<phi::object3D>* _objectsRepository;

private:
    void library::loadTextures(std::string directory);
    void library::loadMaterials(std::string directory);
    void library::loadObjects(std::string directory);

public:
    library(std::string resourcesPath);
    void init();

    phi::resourcesRepository<phi::texture>* getTexturesRepository() const { return _texturesRepository; }
    phi::resourcesRepository<phi::material>* getMaterialsRepository() const { return _materialsRepository; }
    phi::resourcesRepository<phi::object3D>* getObjectsRepository() const { return _objectsRepository; }
};

#endif
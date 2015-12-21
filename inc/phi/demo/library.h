#ifndef _LIBRARY_H_
#define _LIBRARY_H_

#include <phi\core\resourcesRepository.h>
#include <phi\rendering\material.h>

class library
{
private:
    std::string _resourcesPath;
    phi::resourcesRepository<phi::material>* _materialsRepository;
    phi::resourcesRepository<phi::texture> _texturesRepository;

private:
    void library::loadMaterials(std::string directory);
    void library::loadTextures(std::string directory);

public:
    library(std::string resourcesPath);
    void init();
};

#endif
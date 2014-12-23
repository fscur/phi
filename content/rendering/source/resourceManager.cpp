#include "globals.h"
#include "resourceManager.h"
#include <stdio.h>

namespace phi
{
    resourceManager* resourceManager::_instance = NULL;

    resourceManager::resourceManager()
    {
        _textures = new std::map<std::string, texture*>();
        _materials = new std::map<std::string, material*>();
        _fonts = new std::map<std::string, TTF_Font*>();
        _meshes = new std::map<std::string, mesh*>();
    }

    resourceManager::~resourceManager()
    {
    }

    resourceManager* resourceManager::get()
    {
        if (!_instance)
        {
            _instance = new resourceManager();

            _instance->loadTextures();
            _instance->loadMaterials();
            _instance->loadFonts();
            _instance->loadMeshes();
        }

        return _instance;
    }

    bool resourceManager::loadMaterials()
    {
        material* bricks = new material(
            getTexture("diffuse"), //diffuse texture
            //NULL,
            getTexture("normal"), //normal texture
            getTexture("specular"), //specular texture,
            color::white, // ambient color
            color::white, //diffuse color
            color::gray, //specular color
            0.2f, // ambient reflectance coeff
            0.8f, // diffuse reflectance coeff
            0.1f, // specular reflectance coeff
            6.5f); // shininess

        addMaterial("Bricks", bricks);

        material* red = new material(
			nullptr, //diffuse texture
            nullptr, //normal texture
            nullptr, //specular texture,
            color::red, // ambient color
			color::red, //diffuse color
            color::red, //specular color
            0.2f, // ambient reflectance coeff
            1.0f, // diffuse reflectance coeff
            0.0f, // specular reflectance coeff
            0.0f); // shininess

        addMaterial("Red", red);

        material* white = new material(
			nullptr, //diffuse texture
            nullptr, //normal texture
            nullptr, //specular texture,
            color::white, // ambient color
            color::white, //diffuse color
            color::white, //specular color
            0.1f, // ambient reflectance coeff
            0.8f, // diffuse reflectance coeff
            0.3f, // specular reflectance coeff
            64.0f); // shininess

        addMaterial("White", white);

		material* bubble = new material(
            getTexture("BubbleGrip-ColorMap"), //diffuse texture
            getTexture("BubbleGrip-NormalMap"), //normal texture
            nullptr, //specular texture,
            color::red, // ambient color
            color::orange, //diffuse color
            color::orange, //specular color
            0.2f, // ambient reflectance coeff
            0.3f, // diffuse reflectance coeff
            4.8f, // specular reflectance coeff
            8.0f); // shininess

        addMaterial("Bubble", bubble);

        return true;
    }

    bool resourceManager::loadTextures()
    {
        //TODO: Load all textures in Resources/Textures
		addTexture("alpha", texture::fromFile(TEXTURES_PATH + "alpha.bmp"));
        addTexture("button", texture::fromFile(TEXTURES_PATH + "button.png"));
        addTexture("close", texture::fromFile(TEXTURES_PATH + "close.bmp"));
		
		addTexture("default_diffuseMap", texture::fromFile(TEXTURES_PATH + "default_diffuseMap.bmp"));
        addTexture("default_normalMap", texture::fromFile(TEXTURES_PATH + "default_normalMap.bmp"));
        addTexture("default_specularMap", texture::fromFile(TEXTURES_PATH + "default_specularMap.bmp"));
        addTexture("test", texture::fromFile(TEXTURES_PATH + "test.tga"));
		addTexture("diffuse", texture::fromFile(TEXTURES_PATH + "diffuse.bmp"));
		addTexture("normal", texture::fromFile(TEXTURES_PATH + "normal.bmp"));
        addTexture("specular", texture::fromFile(TEXTURES_PATH + "specular.bmp"));

		addTexture("BubbleGrip-ColorMap", texture::fromFile(TEXTURES_PATH + "BubbleGrip-ColorMap.bmp"));
        addTexture("BubbleGrip-NormalMap", texture::fromFile(TEXTURES_PATH + "BubbleGrip-NormalMap.bmp"));

        return true;
    }

    bool resourceManager::loadFonts()
    {
        addFont("Consola", 14);
        addFont("Consola", 16);
        addFont("Consola", 18);
        addFont("Consola", 24);
        addFont("Consola", 100);

        return true;
    }

    bool resourceManager::loadMeshes()
    {
        //TODO: Load meshes from directory

        return true;
    }

    void resourceManager::addTexture(std::string name, texture* texture)
    {
        (*_textures)[name] = texture;
    }

    void resourceManager::addMaterial(std::string name, material* material)
    {
        (*_materials)[name] = material;
    }

    void resourceManager::addFont(std::string name, int size)
    {
        std::string fontName = FONTS_PATH + name + ".ttf";
        TTF_Font* font = TTF_OpenFont(fontName.c_str(), size);
        (*_fonts)[name + std::to_string(size)] = font;
    }

    void resourceManager::addMesh(std::string name, mesh* mesh)
    {
        (*_meshes)[name] = mesh;
    }

    texture* resourceManager::getTexture(std::string name)
    {
        return (*_textures)[name];
    }

    material* resourceManager::getMaterial(std::string name)
    {
        return (*_materials)[name];
    }

    TTF_Font* resourceManager::getFont(std::string name, int size)
    {
        return (*_fonts)[name + std::to_string(size)];
    }

    mesh* resourceManager::getMesh(std::string name)
    {
        return (*_meshes)[name];
    }

    void resourceManager::release()
    {
        releaseMaterials();
        releaseTextures();
        releaseFonts();
        releaseMeshes();

		DELETE(_instance);
    }

    void resourceManager::releaseMaterials()
    {
        if (_materials)
        {
            std::map<std::string, material*>::iterator i;

            for(i = _materials->begin(); i != _materials->end(); i++)
			{
				DELETE(i->second);
			}

            DELETE(_materials);
        }
    }

    void resourceManager::releaseTextures()
    {
        if (_textures)
        {
            std::map<std::string, texture*>::iterator i;

            for(i = _textures->begin(); i != _textures->end(); i++)
            {
                texture* texture = i->second;
                texture->release();

                DELETE(texture);
            }

            DELETE(_textures);
        }
    }

    void resourceManager::releaseFonts()
    {
        if (_fonts)
        {
            for(std::map<std::string, TTF_Font*>::iterator i = _fonts->begin(); i != _fonts->end(); i++)
                TTF_CloseFont(i->second);

            DELETE(_fonts);
        }
    }

    void resourceManager::releaseMeshes()
    {
        if (_meshes)
        {
            std::map<std::string, mesh*>::iterator i;

            for(i = _meshes->begin(); i != _meshes->end(); i++)
			{
                DELETE(i->second);
			}

            DELETE(_meshes);
        }
    }
}
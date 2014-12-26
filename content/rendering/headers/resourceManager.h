#ifndef _PHI_RESOURCE_MANAGER_H_
#define _PHI_RESOURCE_MANAGER_H_

#include <map>
#include "mesh.h"
#include "material.h"
#include <SDL\SDL_ttf.h>

namespace phi
{
	static const std::string TEXTURES_PATH = "./resources/textures/";
	static const std::string FONTS_PATH = "./resources/fonts/";
	static const std::string MODELS_PATH = "./resources/models/";

	struct resourceManagerInfo
	{
	public:
		std::string path;
	};

    class resourceManager
    {
    private:
        static resourceManager* _instance;
        std::map<std::string, mesh*>* _meshes;
        std::map<std::string, texture*>* _textures;
        std::map<std::string, material*>* _materials;
        std::map<std::string, TTF_Font*>* _fonts;
		resourceManagerInfo _info;
    private:
        resourceManager();

        bool loadTextures();
        bool loadMaterials();
        bool loadFonts();
        bool loadMeshes();

        void releaseMaterials();
        void releaseTextures();
        void releaseFonts();
        void releaseMeshes();

    public:
        RENDERING_API ~resourceManager();
        RENDERING_API static resourceManager* get();

		RENDERING_API void init(resourceManagerInfo info);

        RENDERING_API void addTexture(std::string name, texture* texture);
        RENDERING_API void addMaterial(std::string name, material* material);
        RENDERING_API void addFont(std::string name, int size);
        RENDERING_API void addMesh(std::string name, mesh* mesh);

        RENDERING_API texture* getTexture(std::string name);
        RENDERING_API material* getMaterial(std::string name);
        RENDERING_API TTF_Font* getFont(std::string name, int size);
        RENDERING_API mesh* getMesh(std::string name);

        RENDERING_API void release();
    };
}
#endif
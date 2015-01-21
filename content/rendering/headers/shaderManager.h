#ifndef _PHI_SHADER_MANAGER_H_
#define _PHI_SHADER_MANAGER_H_

#include "rendering.h"
#include "shader.h"
#include <map>
#include <string>

namespace phi
{
	static const std::string SHADERS_PATH = "./resources/shaders/";

	struct shaderManagerInfo
	{
	public:
		std::string path;
	};

    class shaderManager
    {
    private:
        static shaderManager* _instance;
        std::map<std::string, shader*>* _shaders;
		shaderManagerInfo _info;
    private: 
        shaderManager();
		void addBasicShader();
		void addFsAmbientLightShader();
		void addFsDirLightShader();
		void addFsDirLightShadowMapShader();
		void addFsPointLightShader();
		void addFsSpotLightShader();
		void addDsGeomPassShader();
		void addDsStencilShader();
		void addDsDirLightShader();
		void addDsSelectedObjectsShader();
		void addDsPointLightShader();
		void addDsSpotLightShader();
		void addRenderToQuadShader();
		void addHudTextShader();
		void addHudQuadShader();
		void addSkyDomeShader();

    public:
        RENDERING_API ~shaderManager();

        RENDERING_API static shaderManager* get();
		
		RENDERING_API void init(shaderManagerInfo info);

        RENDERING_API void addShader(std::string name, shader* shader);
        RENDERING_API shader* getShader(std::string name);
		RENDERING_API shader* loadShader(std::string name, std::string vertFile, std::string fragFile, std::vector<std::string> attributes);
		
        RENDERING_API void release();
    };
}

#endif
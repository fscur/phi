#include "shaderManager.h"

namespace phi
{
	shaderManager* shaderManager::_instance = nullptr;

	shaderManager::shaderManager()
	{
		_shaders = new std::map<std::string, shader*>();
	}

	shaderManager::~shaderManager()
	{
	}

	shaderManager* shaderManager::get()
	{
		if (!_instance)
			_instance = new shaderManager();

		return _instance;
	}

	void shaderManager::init(shaderManagerInfo info)
	{
		_info = info;
		
		addBasicShader();

		addFsAmbientLightShader();
		addFsDirLightShader();
		addFsDirLightShadowMapShader();
		addFsPointLightShader();
		addFsSpotLightShader();

		addDsGeomPassShader();
		addDsStencilShader();
		addDsDirLightShader();
		addDsPointLightShader();
		addDsSpotLightShader();
		addDsSelectedObjectsShader();

		addRenderToQuadShader();
		addHudTextShader();
		addHudQuadShader();
		//addSkyDomeShader();
	}

	void shaderManager::addShader(std::string name, shader* shader)
	{
		if (_shaders->find(name) != _shaders->end())
			return;

		(*_shaders)[name] = shader;
	}

	shader* shaderManager::loadShader(std::string name, std::string vertFile, std::string fragFile, std::vector<std::string> attributes)
	{
		shader* s = new shader(name, _info.path + SHADERS_PATH + vertFile, _info.path + SHADERS_PATH + fragFile, attributes);
		s->init();
		return s;
	}

	void shaderManager::addBasicShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("BASIC", "basic.vert", "basic.frag", attribs);

		s->addUniform("mvp");
		s->addUniform("diffuseMap");
		s->addUniform("diffuseColor");

		addShader(s->getName(), s);
	}

	void shaderManager::addFsAmbientLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("FS_AMBIENT_LIGHT", "fs_ambient_light.vert", "fs_ambient_light.frag", attribs);

		s->addUniform("mvp");
		s->addUniform("ambientLightColor");
		s->addUniform("diffuseMap");
		s->addUniform("mat.ambientColor");
		s->addUniform("mat.ka");

		addShader(s->getName(), s);
	}

	void shaderManager::addFsDirLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		attribs.push_back("inNormal");
		attribs.push_back("inTangent");

		shader* s = loadShader("FS_DIR_LIGHT", "fs_dir_light.vert", "fs_dir_light.frag", attribs);

		s->addUniform("p");
		s->addUniform("v");
		s->addUniform("m");
		s->addUniform("mvp");
		s->addUniform("itmv");

		s->addUniform("light.position");
		s->addUniform("light.color");
		s->addUniform("light.intensity");
		s->addUniform("light.direction");

		s->addUniform("mat.ambientColor");
		s->addUniform("mat.diffuseColor");
		s->addUniform("mat.specularColor");
		s->addUniform("mat.ka");
		s->addUniform("mat.kd");
		s->addUniform("mat.ks");
		s->addUniform("mat.shininess");

		s->addUniform("diffuseMap");
		s->addUniform("normalMap");
		s->addUniform("specularMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addFsDirLightShadowMapShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");

		shader* s = loadShader("FS_DIR_LIGHT_SHADOW_MAP", "fs_dir_light_shadow_map.vert", "fs_dir_light_shadow_map.frag", attribs);

		s->addUniform("mvp");

		addShader(s->getName(), s);
	}

	void shaderManager::addFsPointLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		attribs.push_back("inNormal");
		attribs.push_back("inTangent");

		shader* s = loadShader("FS_POINT_LIGHT", "fs_point_light.vert", "fs_point_light.frag", attribs);

		s->addUniform("p");
		s->addUniform("v");
		s->addUniform("m");
		s->addUniform("mvp");
		s->addUniform("itmv");

		s->addUniform("light.position");
		s->addUniform("light.color");
		s->addUniform("light.intensity");
		s->addUniform("light.attenuation.constant");
		s->addUniform("light.attenuation.linear");
		s->addUniform("light.attenuation.exponential");
		s->addUniform("light.range");

		s->addUniform("mat.diffuseColor");
		s->addUniform("mat.specularColor");
		s->addUniform("mat.kd");
		s->addUniform("mat.ks");
		s->addUniform("mat.shininess");

		s->addUniform("diffuseMap");
		s->addUniform("normalMap");
		s->addUniform("specularMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addFsSpotLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		attribs.push_back("inNormal");
		attribs.push_back("inTangent");

		shader* s = loadShader("FS_SPOT_LIGHT", "fs_spot_light.vert", "fs_spot_light.frag", attribs);

		s->addUniform("p");
		s->addUniform("v");
		s->addUniform("m");
		s->addUniform("mvp");
		s->addUniform("itmv");

		s->addUniform("light.position");
		s->addUniform("light.color");
		s->addUniform("light.intensity");
		s->addUniform("light.attenuation.constant");
		s->addUniform("light.attenuation.linear");
		s->addUniform("light.attenuation.exponential");
		s->addUniform("light.range");
		s->addUniform("light.direction");
		s->addUniform("light.cutoff");

		s->addUniform("mat.diffuseColor");
		s->addUniform("mat.specularColor");
		s->addUniform("mat.kd");
		s->addUniform("mat.ks");
		s->addUniform("mat.shininess");

		s->addUniform("diffuseMap");
		s->addUniform("normalMap");
		s->addUniform("specularMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addDsGeomPassShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		attribs.push_back("inNormal");
		attribs.push_back("inTangent");

		shader* s = loadShader("DS_GEOM_PASS", "ds_geom_pass.vert", "ds_geom_pass.frag", attribs);

		s->addUniform("mv");
		s->addUniform("mvp");
		s->addUniform("itmv");

		s->addUniform("ambientLightColor");

		s->addUniform("mat.ambientColor");
		s->addUniform("mat.diffuseColor");
		s->addUniform("mat.specularColor");
		s->addUniform("mat.ka");
		s->addUniform("mat.kd");
		s->addUniform("mat.ks");
		s->addUniform("mat.shininess");

		s->addUniform("diffuseMap");
		s->addUniform("normalMap");
		s->addUniform("specularMap");

		s->addUniform("isSelected");

		addShader(s->getName(), s);
	}

	void shaderManager::addDsStencilShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");

		shader* s = loadShader("DS_STENCIL", "ds_stencil.vert", "ds_stencil.frag", attribs);

		s->addUniform("mvp");

		addShader(s->getName(), s);
	}

	void shaderManager::addDsDirLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("DS_DIR_LIGHT", "ds_dir_light.vert", "ds_dir_light.frag", attribs);

		s->addUniform("v");
		s->addUniform("m");

		s->addUniform("light.position");
		s->addUniform("light.color");
		s->addUniform("light.intensity");
		s->addUniform("light.direction");

		s->addUniform("positionMap");
		s->addUniform("normalMap");
		s->addUniform("diffuseMap");
		s->addUniform("specularMap");
		s->addUniform("shininessMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addDsSelectedObjectsShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("DS_SELECTED_OBJECTS", "ds_selected_objects.vert", "ds_selected_objects.frag", attribs);

		s->addUniform("m");
		s->addUniform("res");
		s->addUniform("isSelectedMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addDsPointLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");

		shader* s = loadShader("DS_POINT_LIGHT", "ds_point_light.vert", "ds_point_light.frag", attribs);

		s->addUniform("v");
		s->addUniform("m");
		s->addUniform("mvp");
		s->addUniform("res");

		s->addUniform("light.position");
		s->addUniform("light.color");
		s->addUniform("light.intensity");
		s->addUniform("light.attenuation.constant");
		s->addUniform("light.attenuation.linear");
		s->addUniform("light.attenuation.exponential");
		s->addUniform("light.range");

		s->addUniform("positionMap");
		s->addUniform("normalMap");
		s->addUniform("diffuseMap");
		s->addUniform("specularMap");
		s->addUniform("shininessMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addDsSpotLightShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");

		shader* s = loadShader("DS_SPOT_LIGHT", "ds_spot_light.vert", "ds_spot_light.frag", attribs);

		s->addUniform("v");
		s->addUniform("m");
		s->addUniform("mvp");
		s->addUniform("res");

		s->addUniform("light.position");
		s->addUniform("light.color");
		s->addUniform("light.intensity");
		s->addUniform("light.attenuation.constant");
		s->addUniform("light.attenuation.linear");
		s->addUniform("light.attenuation.exponential");
		s->addUniform("light.range");
		s->addUniform("light.direction");
		s->addUniform("light.cutoff");

		s->addUniform("positionMap");
		s->addUniform("normalMap");
		s->addUniform("diffuseMap");
		s->addUniform("specularMap");
		s->addUniform("shininessMap");

		addShader(s->getName(), s);
	}

	void shaderManager::addRenderToQuadShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("RENDER_TO_QUAD", "render_to_quad.vert", "render_to_quad.frag", attribs);

		s->addUniform("quadTexture");

		addShader(s->getName(), s);
	}

	void shaderManager::addHudQuadShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("HUD_QUAD", "hud_quad.vert", "hud_quad.frag", attribs);

		s->addUniform("mvp");
		s->addUniform("quadTexture");
		s->addUniform("backColor");

		addShader(s->getName(), s);
	}

	void shaderManager::addHudTextShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");

		shader* s = loadShader("HUD_TEXT", "hud_text.vert", "hud_text.frag", attribs);

		s->addUniform("mvp");
		s->addUniform("textTexture");

		addShader(s->getName(), s);
	}

	void shaderManager::addSkyDomeShader()
	{
		std::vector<std::string> attribs;
		attribs.push_back("inPosition");
		attribs.push_back("inTexCoord");
		attribs.push_back("inNormal");
		attribs.push_back("inTangent");

		shader* s = loadShader("SKY_DOME", "sky_dome.vert", "sky_dome.frag", attribs);

		s->addUniform("p");
		s->addUniform("v");
		s->addUniform("m");
		s->addUniform("mvp");
		s->addUniform("itmv");
		s->addUniform("time");
		s->addUniform("radius");

		s->addUniform("normalMap");

		addShader(s->getName(), s);
	}

	shader* shaderManager::getShader(std::string name)
	{
		if (_shaders->find(name) == _shaders->end())
			return NULL;
		else
			return (*_shaders)[name];
	}

	void shaderManager::release()
	{
		if (_shaders)
		{
			for(std::map<std::string, shader*>::iterator i = _shaders->begin(); i != _shaders->end(); i++) 
			{
				(i->second)->release();
				delete (i->second);
				(i->second) = NULL;
			}

			delete _shaders;
			_shaders = NULL;
		}

		if (_instance)
		{
			delete _instance;
			_instance = NULL;
		}
	}
}
#include <phi/rendering/shaderManager.h>

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

        addRenderToQuadShader();
        addHudTextShader();
        addHudQuadShader();
        addUIMeshShader();

        addPostSelectedObjectsShader();
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
        s->addUniform("res");
        s->addUniform("texture");
        s->addUniform("color");
        s->addUniform("texCoordOrigin");
        s->addUniform("texCoordQuadSize");
        s->addUniform("texSize");

        addShader(s->getName(), s);
    }

    void shaderManager::addUIMeshShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");

        shader* s = loadShader("UI_MESH", "ui_mesh.vert", "ui_mesh.frag", attribs);

        s->addUniform("mvp");
        s->addUniform("color");

        addShader(s->getName(), s);
    }

    void shaderManager::addPostSelectedObjectsShader()
    {
        std::vector<std::string> attribs;
        attribs.push_back("inPosition");
        attribs.push_back("inTexCoord");

        shader* s = loadShader("POST_SELECTED_OBJECTS", "post_selected_objects.vert", "post_selected_objects.frag", attribs);

        s->addUniform("m");
        s->addUniform("res");
        s->addUniform("selectionMap");

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
                safeDelete(i->second);
            }

            safeDelete(_shaders);
        }

        if (_instance)
            safeDelete(_instance);
    }
}
#include <phi\demo\screen.h>
#include <phi\diagnostics\diagnostics.h>
#include <phi\diagnostics\stopwatch.h>
#include <phi\core\globals.h>
#include <phi\rendering\model.h>

#include <phi\rendering\shaderManager.h>
#include <GLM\gtc\constants.hpp>

screen::screen() : form()
{
    _temp = 0.0f;
    _isMouseDown = false;
    _rotating = false;
    _shadowMap = false;
    _translationSpeed = 1.0f;
    _rotationSpeed = 0.01f;
    _sceneRenderer = nullptr;
}

screen::~screen()
{
}

void screen::initScene()
{
    _scene = new phi::scene(new phi::camera(0.1f, 1000.0f, getSize(), glm::half_pi<float>()));
    auto camera = _scene->getCamera();

    camera->setLocalPosition(phi::vec3(0.0f, 5.0f, 5.0f));
    camera->setTarget(phi::vec3(0.0f, 0.0f, 0.0f));
    camera->update();

    auto vertices = std::vector<phi::vertex>();
    
    vertices.push_back(phi::vertex(phi::vec3(-0.5f, -0.5f, -0.5f), phi::vec2(0.0f, 0.0f), phi::vec3(0.0f, 0.0f, -1.0f)));
    vertices.push_back(phi::vertex(phi::vec3(+0.5f, -0.5f, -0.5f), phi::vec2(1.0f, 0.0f), phi::vec3(0.0f, 0.0f, -1.0f)));
    vertices.push_back(phi::vertex(phi::vec3(+0.5f, +0.5f, -0.5f), phi::vec2(1.0f, 1.0f), phi::vec3(0.0f, 0.0f, -1.0f)));

    auto indices = new std::vector<uint>();
    indices->push_back(0);
    indices->push_back(1);
    indices->push_back(2);

    auto gd = phi::geometryData::create(vertices, indices);
    auto g = new phi::geometry(gd);

    auto triangle0 = new phi::mesh("triangle", g, phi::material::getDefault());

    auto model = new phi::model("cube");
    model->addChild(triangle0);

    _scene->add(model);

    auto info = phi::shaderManagerInfo();
    info.path = _resourcesPath;
    phi::shaderManager::get()->init(info);

    _sceneRenderer = new phi::sceneRenderer();
    _sceneRenderer->init();
}

void screen::onInitialize()
{
    setTitle("Teste");
    centerScreen();
    initScene();
}

void screen::update()
{
    _scene->update();
}

void screen::render()
{
    _sceneRenderer->render(_scene);
}

void screen::onResize(SDL_Event e)
{
    auto sz = phi::sizef((float)e.window.data1, (float)e.window.data2);
    setSize(sz);
    _scene->getCamera()->setResolution(sz);
}

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
}

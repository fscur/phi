#include <phi/loader/importer.h>
#include <phi/demo/screen.h>
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
}

screen::~screen()
{
}

void screen::initScene()
{
    _library = new library(_resourcesPath);
    _library->init();

    _scene = new phi::scene(new phi::camera(0.1f, 1000.0f, getSize(), glm::half_pi<float>()));
    auto camera = _scene->getCamera();

    camera->setLocalPosition(phi::vec3(0.0f, 0.0f, 3.0f));
    camera->setTarget(phi::vec3(0.0f, 0.0f, 0.0f));
    camera->update();

    auto info = phi::shaderManagerInfo();
    info.path = _resourcesPath;
    phi::shaderManager::get()->init(info);


    //auto chair = _library->getObjectsRepository()->getAllResources()[2]->getObject();
    //chair->setLocalPosition(glm::vec3(0.0f, 0.0f, 0.0f));

    //auto cabinet = _library->getObjectsRepository()->getAllResources()[0]->getObject();
    //cabinet->setLocalPosition(glm::vec3(0.0f, 0.0f, 1.5f));

    //auto cabinet2 = _library->getObjectsRepository()->getAllResources()[1]->getObject();
    //cabinet2->setLocalPosition(glm::vec3(0.0f, 0.0f, -1.5f));

    //for(int i = 0; i < 1; i++)
    //{
    //    for(int j = 0; j < 5; j++)
    //    {
    //        auto obj = _library->getObjectsRepository()->getAllResources()[i * 4 + j]->getObject();
    //        obj->setLocalPosition(glm::vec3(i - 2.0f, 0.0f, j - 2.5f));
    //        _scene->add(obj);
    //    }
    //}

    auto obj = _library->getObjectsRepository()->getAllResources()[4]->getObject();
    _scene->add(obj);

    //obj = _library->getObjectsRepository()->getAllResources()[6]->getObject();
    //_scene->add(obj);

    //obj = _library->getObjectsRepository()->getAllResources()[6]->getObject();
    //obj->setLocalPosition(glm::vec3(-2.0f, 0.0f, 0.0f));
    //_scene->add(obj);

    //_scene->add(chair);
    //_scene->add(cabinet);
    //_scene->add(cabinet2);
}

void screen::onInitialize()
{
    setTitle("Φ");
    centerScreen();

    initScene();

    _renderer = new phi::renderer();

    auto staticDrawData = phi::staticDrawData();
    auto camera = _scene->getCamera();
    staticDrawData.projectionMatrix = camera->getViewMatrix();
    staticDrawData.projectionMatrix = camera->getViewMatrix();

    auto renderInfo = phi::renderInfo();
    renderInfo.materials = _library->getMaterialsRepository()->getAllObjects();
    renderInfo.renderList = _scene->getRenderList();

    renderInfo.staticDrawData = staticDrawData;

    _renderer->init(renderInfo);
}

float t = 0.0f;

void screen::update()
{
    t += 0.001f;
    _scene->getCamera()->setLocalPosition(phi::vec3(glm::cos(t), 0.5f, glm::sin(t)) * 2.0f);
    _scene->getCamera()->update();
    _scene->update();
}

void screen::render()
{
    auto sec = phi::stopwatch::measure([&]
    {
        _renderer->render();
    });
    //std::cout << std::to_string(sec * 1000.0f) << std::endl;
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
    //TODO: Check if we really need the above TODO
}
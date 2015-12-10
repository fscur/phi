#include <phi/demo/screen.h>

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
    _scene = new phi::scene(new phi::camera(0.1f, 1000.0f, getSize(), 0.78f));
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

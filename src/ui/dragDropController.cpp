#include "phi/ui/dragDropController.h"
#include "phi/core/input.h"

namespace phi
{
    dragDropController* dragDropController::_instance = nullptr;

    dragDropController::dragDropController()
    {
        _isDragging = false;
        _dragDropEnded = new eventHandler<dragDropEventArgs*>();
        _dragTexture = nullptr;
        _dragTextureRenderer = new quadRenderer2D(vec2(), 10.0f, sizef((GLuint)0.0f, (GLuint)0.0f), sizef((GLuint)0.0f, (GLuint)0.0f));
        input::mouseUp->bind<dragDropController, &dragDropController::inputMouseUp>(this);
        input::mouseMove->bind<dragDropController, &dragDropController::inputMouseMove>(this);
    }

    dragDropController* phi::dragDropController::get()
    {
        if (_instance == nullptr)
            _instance = new dragDropController();

        return _instance;
    }

    void dragDropController::init(sizef viewportSize)
    {
        _viewportSize = viewportSize;
        _dragTextureRenderer->setViewportSize(_viewportSize);
        _dragTextureRenderer->setSize(sizef((GLuint)128.0f, (GLuint)128.0f));
        _dragTextureRenderer->update();
    }
    
    void dragDropController::startDrag(std::string data, texture* texture)
    {
        _isDragging = true;
        _dragData = data;
        _dragTexture = texture;
    }

    void dragDropController::inputMouseUp(mouseEventArgs* e)
    {
        if (_isDragging && e->leftButtonPressed)
        {
            _isDragging = false;
            if (_dragDropEnded->isBound())
                _dragDropEnded->invoke(new dragDropEventArgs(_dragData, e->x, e->y));
        }
    }

    void dragDropController::inputMouseMove(mouseEventArgs* e)
    {
        auto size = _dragTextureRenderer->getSize();
        _dragTextureRenderer->setLocation(vec2(e->x, e->y));
        _dragTextureRenderer->update();
    }

    void dragDropController::render()
    {
        if (_isDragging && _dragTexture != nullptr)
            _dragTextureRenderer->render(_dragTexture, color::white);
    }
}
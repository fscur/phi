#include <precompiled.h>
#include "dragDropController.h"

#include <core\input.h>

namespace phi
{
    dragDropController* dragDropController::_instance = nullptr;

    dragDropController::dragDropController()
    {
        _isDragging = false;
        _dragDropEnded = new eventHandler<dragDropEventArgs*>();
        _dragTexture = nullptr;
        _dragTextureRenderer = new quadRenderer2D(vec2(), 10.0f, sizeui(0, 0), sizeui(0, 0));
        input::mouseUp->assign(std::bind(&dragDropController::inputMouseUp, this, std::placeholders::_1));
        input::mouseMove->assign(std::bind(&dragDropController::inputMouseMove, this, std::placeholders::_1));
    }

    dragDropController* phi::dragDropController::get()
    {
        if (_instance == nullptr)
            _instance = new dragDropController();

        return _instance;
    }

    void dragDropController::init(sizeui viewportSize)
    {
        _viewportSize = viewportSize;
        _dragTextureRenderer->setViewportSize(sizeui(_viewportSize.w, viewportSize.h, viewportSize.d));
        _dragTextureRenderer->setSize(sizeui((GLuint)128.0f, (GLuint)128.0f));
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
            _dragDropEnded->raise(new dragDropEventArgs(_dragData, e->x, e->y));
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
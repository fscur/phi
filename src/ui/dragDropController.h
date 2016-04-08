#pragma once
#include <phi.h>

#include <core\size.h>
#include <core\mouseEventArgs.h>
#include <core\eventHandler.h>

#include <rendering\texture.h>
#include <rendering\quadRenderer2D.h>

#include "uiApi.h"
#include "dragDropEventArgs.h"

namespace phi
{
    class dragDropController
    {
    private:
        static dragDropController* _instance;
        sizeui _viewportSize;
        bool _isDragging;
        std::string _dragData;
        texture* _dragTexture;
        eventHandler<dragDropEventArgs*>* _dragDropEnded;
        quadRenderer2D* _dragTextureRenderer;

    private:
        dragDropController();
        void inputMouseUp(mouseEventArgs* e);
        void inputMouseMove(mouseEventArgs* e);

    public:
        static dragDropController* get();

        void init(sizeui viewportSize);

        bool getIsDragging() { return _isDragging; }
        std::string getDragData() { return _dragData; }
        eventHandler<dragDropEventArgs*>* getDradDropEnded() { return _dragDropEnded; }

        void startDrag(std::string value, texture* texture = nullptr);

        void render();
    };
}
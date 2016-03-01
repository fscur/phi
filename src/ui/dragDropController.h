#ifndef _PHI_DRAG_DROP_CONTROLLER_H_
#define _PHI_DRAG_DROP_CONTROLLER_H_

#include <core\size.h>
#include <core\mouseEventArgs.h>
#include <core\eventHandler.h>

#include <rendering\texture.h>
#include <rendering\quadRenderer2D.h>

#include "ui.h"
#include "dragDropEventArgs.h"

#if WIN32
#include <GL\glew.h>
#else
#include <OpenGL\gl3.h>
#endif
#include <string>

namespace phi
{
    class dragDropController
    {
    private:
        static dragDropController* _instance;
        sizef _viewportSize;
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
        UI_API static dragDropController* get();

        UI_API void init(sizef viewportSize);

        UI_API bool getIsDragging() { return _isDragging; }
        UI_API std::string getDragData() { return _dragData; }
        UI_API eventHandler<dragDropEventArgs*>* getDradDropEnded() { return _dragDropEnded; }

        UI_API void startDrag(std::string value, texture* texture = nullptr);

        UI_API void render();
    };
}

#endif
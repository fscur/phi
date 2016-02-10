#ifndef _PHI_TRANSLATION_CONTROL_H_
#define _PHI_TRANSLATION_CONTROL_H_

#include "control.h"
#include "translationEventArgs.h"

#include <phi/core/object3D.h>
#include <phi/core/aabb.h>

#include <phi/rendering/geometry.h>
#include <phi/rendering/transform.h>
#include <phi/rendering/ray.h>
#include <phi/rendering/frustum.h>
#include <phi/rendering/shader.h>

#include <phi/rendering/camera.h>

namespace phi
{
    class translationControl :
        public control
    {
    private:
        geometry* _arrowGeometry;
        mat4 _xModelMatrix;
        mat4 _yModelMatrix;
        mat4 _zModelMatrix;
        aabb* _xAabb;
        aabb* _yAabb;
        aabb* _zAabb;
        bool _mouseOverX;
        bool _mouseOverY;
        bool _mouseOverZ;
        bool _clickedOverX;
        bool _clickedOverY;
        bool _clickedOverZ;
        color _xColor;
        color _yColor;
        color _zColor;
        object3D* _object;
        shader* _shader;
        vec3 _startPos;
        vec3 _startLocalPos;
        vec2 _mouseStartPos;
        camera* _camera;
        mat4 _modelMatrix;
        eventHandler<translationEventArgs*>* _translating;
        eventHandler<translationEventArgs*>* _translationFinished;

    private:
        geometry* createArrowGeometry();
        void updateAabbs();
        void updateModelMatrix();
        void renderArrow(geometry* geometry, color color, mat4 modelMatrix);
        ray castRay(vec2 screenCoords, sizef screenSize);

        vec3 screenToViewZNear(vec2 mousePos);
        vec2 worldToScreen(vec3 worldPos);
        int lineLineIntersect(vec3 p1, vec3 p2, vec3 p3, vec3 p4, vec3 *pa, vec3 *pb, float *mua, float *mub);
        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseLeave(mouseEventArgs* e) override;

    public:
        UI_API translationControl(sizef viewportSize);

        UI_API eventHandler<translationEventArgs*>* getTranslating() const { return _translating; }
        UI_API eventHandler<translationEventArgs*>* getTranslationFinished() const { return _translationFinished; }

        UI_API void setCamera(camera* value) { _camera = value; }

        UI_API virtual bool isPointInside(int x, int y) override;

        UI_API void attachTo(object3D* object);

        UI_API void onRender() override;
    };
}

#endif
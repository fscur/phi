#ifndef _PHI_TRANSLATION_CONTROL_H_
#define _PHI_TRANSLATION_CONTROL_H_

#include "phi/ui/control.h"
#include "phi/rendering/mesh.h"
#include "phi/rendering/transform.h"
#include "phi/core/object3D.h"
#include "phi/rendering/shader.h"
#include "phi/core/aabb.h"
#include "phi/rendering/ray.h"
#include "phi/scenes/camera.h"
#include "phi/rendering/frustum.h"
#include "phi/ui/translationEventArgs.h"

namespace phi
{
    class translationControl :
        public control
    {
    private:
        mesh* _arrowMesh;
        glm::mat4 _xModelMatrix;
        glm::mat4 _yModelMatrix;
        glm::mat4 _zModelMatrix;
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
        glm::vec3 _startPos;
        glm::vec3 _startLocalPos;
        glm::vec2 _mouseStartPos;
        camera* _camera;
        glm::mat4 _modelMatrix;
        eventHandler<translationEventArgs>* _translationFinished;

    private:
        mesh* createArrowMesh();
        void updateAabbs();
        void updateModelMatrix();
        void renderArrow(mesh* mesh, color color, glm::mat4 modelMatrix);
        ray castRay(glm::vec2 screenCoords, size<unsigned int> screenSize);

        glm::vec3 screenToViewZNear(glm::vec2 mousePos);
        glm::vec2 worldToScreen(glm::vec3 worldPos);
        int lineLineIntersect(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 *pa, glm::vec3 *pb, float *mua, float *mub);
        virtual void onMouseDown(mouseEventArgs* e) override;
        virtual void onMouseUp(mouseEventArgs* e) override;
        virtual void onMouseMove(mouseEventArgs* e) override;
        virtual void onMouseLeave(mouseEventArgs* e) override;

    public:
        UI_API translationControl(size<GLuint> viewportSize);
        UI_API ~translationControl();

        UI_API eventHandler<translationEventArgs>* getTranslationFinished() { return _translationFinished; }

        UI_API void setCamera(camera* value) { _camera = value; }

        UI_API virtual bool isPointInside(int x, int y) override;

        UI_API void attachTo(object3D* object);

        UI_API void onRender() override;
    };
}

#endif
#ifndef _PHI_BUTTON_3D_H_
#define _PHI_BUTTON_3D_H_

#include "ui.h"
#include "control.h"
#include "mesh.h"
#include "shader.h"
#include "camera.h"

namespace phi
{
    class button3D :
        public control
    {
    private:
        mesh* _mesh;
        shader* _shader;
        glm::mat4 _modelMatrix;
        object3D* _object;
        camera* _camera;

    private:
        void createMesh();
        void updateModelMatrix();

    public:
        UI_API button3D(size<GLuint> viewportSize);
        UI_API void setCamera(camera* value) { _camera = value; }
        UI_API void attachTo(object3D* object);
        UI_API void onRender() override;
    };
}

#endif
#ifndef _PHI_BUTTON_3D_H_
#define _PHI_BUTTON_3D_H_

#include "phi/rendering/mesh.h"
#include "phi/rendering/shader.h"

#include "phi/scenes/camera.h"

#include "phi/ui/control.h"
#include "phi/ui/ui.h"

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
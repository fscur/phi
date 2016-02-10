#ifndef _PHI_BUTTON_3D_H_
#define _PHI_BUTTON_3D_H_

#include "phi/rendering/geometry.h"
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
        geometry* _geometry;
        shader* _shader;
        mat4 _modelMatrix;
        object3D* _object;
        camera* _camera;

    private:
        void createMesh();
        void updateModelMatrix();

    public:
        UI_API button3D(sizef viewportSize);
        UI_API void setCamera(camera* value) { _camera = value; }
        UI_API void attachTo(object3D* object);
        UI_API void onRender() override;
    };
}

#endif
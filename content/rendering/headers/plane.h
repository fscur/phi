#ifndef _PHI_PLANE_H_
#define _PHI_PLANE_H_

#include "sceneObject.h"

namespace phi
{
    class plane :
        public sceneObject
    {
        private:
            glm::vec3 _normal;
            std::vector<glm::vec3> _points;
        private:
            plane();
        public:
            RENDERING_API plane(glm::vec3 normal, glm::vec3 position, size<float> size, material* material);
            RENDERING_API ~plane(void);

            RENDERING_API glm::vec3 getNormal() const { return _up; }
            RENDERING_API void setNormal(glm::vec3 normal);

            RENDERING_API void addPoint(glm::vec3 point);

            RENDERING_API void debugRender() override;
            RENDERING_API void update() override;
    };
}

#endif
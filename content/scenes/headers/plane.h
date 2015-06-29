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
            mesh* create(float width, float depth);
        public:
            SCENES_API plane(glm::vec3 normal, float width, float depth, material* material);
            SCENES_API ~plane(void);

            SCENES_API glm::vec3 getNormal() const { return _normal; }
            SCENES_API void setNormal(glm::vec3 normal);

            SCENES_API void addPoint(glm::vec3 point);

            SCENES_API void debugRender() override;
            SCENES_API void update() override;
    };
}

#endif
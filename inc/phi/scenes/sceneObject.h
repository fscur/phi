#ifndef _PHI_SCENEOBJECT_H_
#define _PHI_SCENEOBJECT_H_

#include "phi/core/size.h"
#include "phi/core/aabb.h"
#include "phi/core/object3D.h"
#include "phi/core/eventHandler.h"
#include "phi/core/eventArgs.h"

#include "phi/rendering/material.h"
#include "phi/rendering/model.h"

#include "phi/scenes/scenes.h"
#include "phi/scenes/sceneObjectEventArgs.h"
#include "phi/scenes/sceneMesh.h"

#include <list>

namespace phi
{
    class sceneObject :
        public object3D
    {
    private:

        bool _isActive;
        bool _isSelected;
        bool _isInitialized;
        eventHandler<sceneObjectEventArgs>* _isSelectedChanged;

        std::vector<glm::vec3> _points;

        unsigned int _id;

        std::vector<sceneMesh*> _sceneMeshes;

    protected:
        model* _model;

    private:
        void initPoints();
        void updateAabb();

    public:
        SCENES_API sceneObject();
        SCENES_API virtual ~sceneObject(void);

        SCENES_API bool getSelected() const { return _isSelected; }
        SCENES_API bool getActive() const { return _isActive; }
        SCENES_API unsigned int getId() const { return _id; }
        SCENES_API eventHandler<sceneObjectEventArgs>* getIsSelectedChanged() { return _isSelectedChanged; }
        SCENES_API std::vector<sceneMesh*> getSceneMeshes() { return _sceneMeshes; }
        SCENES_API model* getModel() { return _model; }

        SCENES_API void setSelected(bool value);
        SCENES_API void setActive(bool value);
        SCENES_API void setId(unsigned int value) { _id = value; }

        SCENES_API void initialize();

        SCENES_API void render();
        SCENES_API virtual void debugRender();

        SCENES_API virtual void update() override;

        SCENES_API static sceneObject* create(model* model);

        SCENES_API void selectMesh(GLuint meshId);
    };
}
#endif
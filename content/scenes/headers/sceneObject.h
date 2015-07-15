#ifndef _PHI_SCENEOBJECT_H_
#define _PHI_SCENEOBJECT_H_

#include "scenes.h"
#include "material.h"
#include "model.h"
#include "size.h"
#include "aabb.h"
#include "object3D.h"
#include "eventHandler.h"
#include "eventArgs.h"
#include "sceneObjectEventArgs.h"
#include "sceneMesh.h"
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
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
#include <list>

namespace phi
{
    class sceneObject :
        public object3D
    {
    private:

        std::vector<sceneObject*> _children;
        unsigned int _childrenCount;
        bool _isActive;
        bool _isSelected;
        bool _isInitialized;
        aabb* _aabb;
        eventHandler<sceneObjectEventArgs>* _isSelectedChanged;

        std::vector<glm::vec3> _points;

        unsigned int _id;

    protected:
        model* _model;
        bool _changed;

    private:
        void initPoints();
        void updateAabb();

    public:
        SCENES_API sceneObject();
        SCENES_API virtual ~sceneObject(void);

        SCENES_API std::vector<sceneObject*> getChildren() const { return _children; }
        SCENES_API model* getModel() const { return _model; }
        SCENES_API aabb* getAabb() const { return _aabb; }
        SCENES_API virtual bool getChanged() const { return _changed; }
        SCENES_API bool getSelected() const { return _isSelected; }
        SCENES_API unsigned int getId() const { return _id; }
        SCENES_API eventHandler<sceneObjectEventArgs>* getIsSelectedChanged() { return _isSelectedChanged; }

        SCENES_API void setSelected(bool value);
        SCENES_API void setId(unsigned int value) { _id = value; }

        SCENES_API void initialize();

        SCENES_API void addChild(sceneObject* child);

        SCENES_API void render();
        SCENES_API virtual void debugRender();

        SCENES_API virtual void update() override;

        SCENES_API static sceneObject* create(model* model);

        SCENES_API void selectMesh(GLuint meshId);
    };
}
#endif
#ifndef _PHI_SCENEOBJECT_H_
#define _PHI_SCENEOBJECT_H_

#include "scenes.h"
#include "material.h"
#include "mesh.h"
#include "transform.h"
#include "size.h"
#include "aabb.h"

#include <list>

namespace phi
{
    class sceneObject
    {
    private:
        size<float> _size;
        material* _material;

        float _roll;
        float _pitch;
        float _yaw;
    
        std::vector<sceneObject*> _children;
        unsigned int _childrenCount;
        aabb* _aabb;
        bool _isActive;
        bool _isSelected;
        bool _isInitialized;

        std::vector<glm::vec3> _points;
        glm::mat4 _modelMatrix;

		unsigned int _sceneId;

    protected:
        glm::vec3 _position;
        glm::vec3 _right;
        glm::vec3 _up;
        glm::vec3 _direction;
        mesh *_mesh;
        transform* _transform; 
        bool _changed;

    private:
        void initPoints();
        void updateAabb();
        void update(glm::mat4 transform);
    protected:
        sceneObject(){}
    public:
        SCENES_API sceneObject(glm::vec3 position, size<float> size, material* material);
        SCENES_API virtual ~sceneObject(void);

        SCENES_API glm::vec3 getPosition() const { return _position; }
        SCENES_API size<float> getSize() const { return _size ; }
        SCENES_API glm::vec3 getDirection() const { return _direction; }
        SCENES_API glm::vec3 getRight() const { return _right; }
        SCENES_API glm::vec3 getUp() const { return _up; }
        SCENES_API material* getMaterial() const { return _material; }
        SCENES_API std::vector<sceneObject*> getChildren() const { return _children; }
        SCENES_API transform* getTransform() const { return _transform; }
        SCENES_API mesh* getMesh() const { return _mesh; }
        SCENES_API aabb* getAabb() const { return _aabb; }
		SCENES_API virtual bool getChanged() const { return _changed; }
		SCENES_API bool getSelected() const { return _isSelected; }
        SCENES_API unsigned int getSceneId() const { return _sceneId; }
        
        SCENES_API void setSelected(bool value) { _isSelected = value; }
		SCENES_API void setMaterial(material* value) { _material = value; _changed = true; }
		SCENES_API void setPosition(glm::vec3 value) { _position = value; _changed = true; }
		SCENES_API void setSize(size<float> value) { _size = value; _changed = true; }
		SCENES_API void setDirection(glm::vec3 direction);
		SCENES_API void setSceneId(unsigned int value) { _sceneId = value; }
        
        SCENES_API void initialize();

        SCENES_API void addChild(sceneObject* child);

        SCENES_API virtual void update();
        SCENES_API void render();
        SCENES_API virtual void debugRender();
    
        SCENES_API void translate(glm::vec3 translation);
        SCENES_API void roll(float angle);
        SCENES_API void pitch(float angle);
        SCENES_API void yaw(float angle);
    };
}
#endif
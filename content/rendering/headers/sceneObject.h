#ifndef _PHI_SCENEOBJECT_H_
#define _PHI_SCENEOBJECT_H_

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
        RENDERING_API sceneObject(glm::vec3 position, size<float> size, material* material);
        RENDERING_API virtual ~sceneObject(void);

        RENDERING_API glm::vec3 getPosition() const { return _position; }
        RENDERING_API size<float> getSize() const { return _size ; }
        RENDERING_API glm::vec3 getDirection() const { return _direction; }
        RENDERING_API glm::vec3 getRight() const { return _right; }
        RENDERING_API glm::vec3 getUp() const { return _up; }
        RENDERING_API material* getMaterial() const { return _material; }
        RENDERING_API std::vector<sceneObject*> getChildren() const { return _children; }
        RENDERING_API transform* getTransform() const { return _transform; }
        RENDERING_API mesh* getMesh() const { return _mesh; }
        RENDERING_API aabb* getAabb() const { return _aabb; }
		RENDERING_API virtual bool getChanged() const { return _changed; }
		RENDERING_API bool getSelected() const { return _isSelected; }
        
        RENDERING_API void setSelected(bool isSelected) { _isSelected = isSelected; }
		RENDERING_API void setMaterial(material* material) { _material = material; _changed = true; }
		RENDERING_API void setPosition(glm::vec3 position) { _position = position; _changed = true; }
		RENDERING_API void setSize(size<float> size) { _size = size; _changed = true; }
		RENDERING_API void setDirection(glm::vec3 direction);

        RENDERING_API void initialize();

        RENDERING_API void addChild(sceneObject* child);

        RENDERING_API virtual void update();
        RENDERING_API void render();
        RENDERING_API virtual void debugRender();
    
        RENDERING_API void translate(glm::vec3 translation);
        RENDERING_API void roll(float angle);
        RENDERING_API void pitch(float angle);
        RENDERING_API void yaw(float angle);
    };
}
#endif
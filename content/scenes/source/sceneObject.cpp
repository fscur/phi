#include "SceneObject.h"
#include "MathUtils.h"
#include "Globals.h"
#include "MeshRenderer.h"

namespace phi
{
    sceneObject::sceneObject() :
        object3D()
    {
        _id = 0;

        _isSelected = false;
        _isActive = true;
        _isInitialized = false;
        setAabb(new aabb(glm::vec3(), glm::vec3()));
        _isSelectedChanged = new eventHandler<sceneObjectEventArgs>();
    }

    sceneObject::~sceneObject(void)
    {
    }

    void sceneObject::initialize()
    {
        /*initPoints();
        _aabb = new aabb(_points);*/
    }

    void sceneObject::initPoints()
    {
        _points = std::vector<glm::vec3>();
        auto meshesCount = _model->getMeshes().size();
        for (auto i = 0; i < meshesCount; i++)
        {
            std::vector<vertex> vertices = _model->getMeshes()[i]->getVertices();
            auto verticesCount = vertices.size();

            for (unsigned int i = 0; i < verticesCount; i++)
                _points.push_back(vertices[i].getPosition());
        }
    }

    void sceneObject::setSelected(bool value)
    {
        _isSelected = value;

        if (_isSelectedChanged->isBound())
            _isSelectedChanged->invoke(sceneObjectEventArgs(this));
    }

    void sceneObject::setActive(bool value)
    {
        auto oldValue = _isActive;
        _isActive = value;
        if (oldValue && !value)
            setSelected(false);
    }

    void sceneObject::render()
    {
    }

    void sceneObject::update()
    {
        object3D::update();

        //updateAabb();
    }

    void sceneObject::debugRender()
    {
#ifdef WIN32
        /*vector<glm::vec3> points;
        vector<Vertex> vertices = _mesh->GetVertices();
        glm::mat4 modelMatrix = _transform->GetModelMatrix();

        for (unsigned int i = 0; i < vertices.size(); i++)
        {
        glm::vec3 point = vertices[i].GetPosition();
        glm::vec4 p = modelMatrix * glm::vec4(point.x, point.y, point.z, 1);
        points.push_back(glm::vec3(p.x, p.y, p.z)/p.w);
        }

        glBegin(GL_LINES);
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.0f);
        for (int i = 0; i < points.size(); i++)
        {   
        int j = i == points.size() - 1 ? 0 : i + 1;
        glm::vec3 p0 = points[i];
        glm::vec3 p1 = points[j];
        glVertex3f(p0.x, p0.y, p0.z); glVertex3f(p1.x, p1.y, p1.z);
        }

        glPopMatrix();
        glEnd();*/

        //bounding box
        /*
        glm::vec3 min = _aabb->getMin();
        glm::vec3 max = _aabb->getMax();
        glm::vec3 center = _aabb->getCenter();
        float radius = _aabb->getRadius();

        color color = _material->getDiffuseColor();

        glBegin(GL_LINES);

        if (_isSelected)
        glColor3f(1.0f, 0.0f, 0.0f);
        else
        glColor3f(color.g, color.g, color.b);

        glPushMatrix();
        // front quad
        glVertex3f(min.x, min.y, min.z); glVertex3f(max.x, min.y, min.z);
        glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, max.y, min.z);
        glVertex3f(max.x, max.y, min.z); glVertex3f(min.x, max.y, min.z); 
        glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, min.y, min.z);
        //back quad
        glVertex3f(min.x, min.y, max.z); glVertex3f(max.x, min.y, max.z);
        glVertex3f(max.x, min.y, max.z); glVertex3f(max.x, max.y, max.z);
        glVertex3f(max.x, max.y, max.z); glVertex3f(min.x, max.y, max.z); 
        glVertex3f(min.x, max.y, max.z); glVertex3f(min.x, min.y, max.z);
        //connect lines
        glVertex3f(min.x, min.y, min.z); glVertex3f(min.x, min.y, max.z);
        glVertex3f(max.x, min.y, min.z); glVertex3f(max.x, min.y, max.z);
        glVertex3f(max.x, max.y, min.z); glVertex3f(max.x, max.y, max.z); 
        glVertex3f(min.x, max.y, min.z); glVertex3f(min.x, max.y, max.z);

        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(center.x, center.y, center.z); glVertex3f(center.x + _right.x, center.y + _right.y, center.z + _right.z);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(center.x, center.y, center.z); glVertex3f(center.x + _up.x, center.y + _up.y, center.z + _up.z);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(center.x, center.y, center.z); glVertex3f(center.x + _direction.x, center.y + _direction.y, center.z + _direction.z);

        //bounding sphere
        /*glColor3f(0.0f, 1.0f, 1.0f);

        float rings = 36;
        float sectors = 36;

        float const R = 1./(float)(rings-1);
        float const S = 1./(float)(sectors-1);
        int r, s;

        vector<glm::vec3> points;
        float pi = PI;
        float pi_2 = PI/2.0f;

        for(r = 0; r < rings; r++) 
        {
        for(s = 0; s < sectors; s++) 
        {
        float const y = sin(-pi_2 + pi * r * R );
        float const x = cos(2*pi * s * S) * sin( pi * r * R );
        float const z = sin(2*pi * s * S) * sin( pi * r * R );

        points.push_back(center + glm::vec3(x * radius, y * radius, z * radius));
        }
        }

        glColor3f(1.0f, 0.0f, 1.0f);
        for (int i = 0; i < points.size(); i++)
        {   
        int j = i == points.size() - 1 ? 0 : i + 1;
        glm::vec3 p0 = points[i];
        glm::vec3 p1 = points[j];
        glVertex3f(p0.x, p0.y, p0.z); glVertex3f(p1.x, p1.y, p1.z);
        }*/

        //glPopMatrix();

        //glEnd();

#endif
    }

    sceneObject* sceneObject::create(model* model)
    {
        sceneObject* sceneObj = new sceneObject();

        std::vector<sceneMesh*> sceneMeshes;
        auto meshesCount = model->getMeshes().size();
        for (unsigned int i = 0; i < meshesCount; i++)
        {
            auto mesh = model->getMeshes()[i];
            auto sm = new sceneMesh(mesh->getId());
            sm->setMaterial(mesh->getMaterial());
            sceneMeshes.push_back(sm);
        }
        sceneObj->_sceneMeshes = sceneMeshes;
        sceneObj->_model = model;
        sceneObj->initPoints();
        return sceneObj;
    }

    void sceneObject::selectMesh(GLuint meshId)
    {
        auto meshesCount = _sceneMeshes.size();

        for (unsigned int i = 0; i < meshesCount; i++)
        {
            auto m = _sceneMeshes[i];

            if (m->getMeshID() == meshId && !m->getIsSelected())
                m->setIsSelected(true);
            else
                m->setIsSelected(false);
        }
    }

    void sceneObject::updateAabb()
    {
        unsigned int count = _points.size();

        if (count == 0)
            return;

        std::vector<glm::vec3> points = std::vector<glm::vec3>(count);

        for (unsigned int i = 0; i < count; i++)
        {
            glm::vec3 point = _points[i];
            glm::vec4 p = getModelMatrix() * glm::vec4(point.x, point.y, point.z, 1);
            points[i] = glm::vec3(p.x, p.y, p.z)/p.w;
        }

        getAabb()->update(points);
    }
}
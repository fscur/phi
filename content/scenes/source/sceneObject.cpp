#include "SceneObject.h"
#include "MathUtils.h"
#include "Globals.h"
#include "MeshRenderer.h"

namespace phi
{
    sceneObject::sceneObject(glm::vec3 position, size<float> size, material* material)
    {
		_sceneId = 0;
        _position = position;
        _size = size;
        _material = material;
        
        _yaw = 0;
        _roll = 0;
        _pitch = 0;

        _right = glm::vec3(1.0f, 0.0f, 0.0f);
        _up = glm::vec3(0.0f, 1.0f, 0.0f);
        _direction = glm::vec3(0.0f, 0.0f, 1.0f);
        _transform = new transform();
        _childrenCount = 0;
        _isSelected = false;
        _isActive = true;
        _changed = true;
        _isInitialized = false;
		_aabb = nullptr;
    }

    sceneObject::~sceneObject(void)
    {
        for (unsigned int i = 0; i < _childrenCount; i++)
            DELETE(_children[i]);

        DELETE(_transform);
        DELETE(_aabb);
    }

	void sceneObject::setDirection(glm::vec3 direction) 
    {
        _direction = glm::normalize(direction); 
        _right = glm::normalize(_right - _direction * glm::dot(_direction, _right));
        _up = glm::cross(_direction, _right);
        _changed = true;
    }

    void sceneObject::initialize()
    {
        initPoints();
        _aabb = new aabb(_points);
    }

    void sceneObject::initPoints()
    {
        std::vector<vertex> vertices = _mesh->getVertices();
        unsigned int count = vertices.size();
        _points.resize(count);

        for (unsigned int i = 0; i < count; i++)
            _points[i] = vertices[i].getPosition();
    }

	void sceneObject::addChild(sceneObject* child)
    {
        _children.push_back(child);
        _childrenCount++;
    }

    void sceneObject::update()
    {
        if (_changed)
        {
            update(glm::mat4(1.0f));

            _changed = false;
        }
    }

    void sceneObject::update(glm::mat4 parentModelMatrix)
    {
        glm::mat4 rotation = glm::mat4(
                _right.x,     _right.y,     _right.z, 0.0f,
                   _up.x,        _up.y,        _up.z, 0.0f,
            _direction.x, _direction.y, _direction.z, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

		glm::mat4 scale = glm::mat4(
            _size.width, 0.0f, 0.0f, 0.0f,
            0.0f, _size.height, 0.0f, 0.0f,
            0.0f, 0.0f, _size.depth, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        glm::mat4 translation = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            _position.x, _position.y, _position.z, 1.0f);

        _modelMatrix = parentModelMatrix * translation  * rotation * scale;

        glm::mat3 normalMatrix = glm::mat3(
                _modelMatrix[0][0], _modelMatrix[0][1], _modelMatrix[0][2],
                _modelMatrix[1][0], _modelMatrix[1][1], _modelMatrix[1][2],
                _modelMatrix[2][0], _modelMatrix[2][1], _modelMatrix[2][2]);

        _transform->setModelMatrix(_modelMatrix);

        updateAabb();

        for (unsigned int i = 0; i < _childrenCount; i++)
            _children[i]->update(_modelMatrix);
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
            glm::vec4 p = _modelMatrix * glm::vec4(point.x, point.y, point.z, 1);
            points[i] = glm::vec3(p.x, p.y, p.z)/p.w;
        }

        _aabb->update(points);
    }

    void sceneObject::render()
    {
        meshRenderer::render(_mesh);

        for (unsigned int i = 0; i < _childrenCount; i++)
            _children[i]->render();
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

        glPopMatrix();

        glEnd();
#endif
    }

    void sceneObject::translate(glm::vec3 translation)
    {
        _position += translation;
        _changed = true;
    }

    void sceneObject::pitch(float angle)
    {
        _pitch += angle;

        _direction = mathUtils::rotateAboutAxis(_direction, _right, angle);
        _direction = glm::normalize(_direction);

        _up = glm::cross(_direction, _right);
        _right = glm::cross(_up, _direction);
    
        _right = glm::normalize(_right);
        _up = glm::normalize(_up);
        _changed = true;
    }

    void sceneObject::yaw(float angle)
    {
        _yaw += angle;

        _direction = mathUtils::rotateAboutAxis(_direction, _up, angle);
        _direction = glm::normalize(_direction);

        _right = glm::cross(_up, _direction);
        _up = glm::cross(_direction, _right);

        _up = glm::normalize(_up);
        _right = glm::normalize(_right);
        _changed = true;
    }

    void sceneObject::roll(float angle)
    {
        _roll += angle;

        _up = mathUtils::rotateAboutAxis(_up, _direction, angle);
        _up = glm::normalize(_up);

        _right = glm::cross(_up, _direction);
        _direction = glm::cross(_right, _up);

        _right = glm::normalize(_right);
        _direction = glm::normalize(_direction);
        _changed = true;
    }
}
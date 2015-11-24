#include <phi/scenes/plane.h>

#include <glm/gtx/vector_angle.hpp>

namespace phi
{
    plane::plane()
    {
    }

    plane::~plane()
    {
    }

    plane::plane(glm::vec3 normal, float width, float depth, material* material)
        : sceneObject()
    {
        setSize(size<float>(width, 0.01f, depth));

        auto m = create(width, depth);
        m->setMaterial(material);
        m->setId(0);
        _model = new model("plane", "");
        _model->addMesh(m);

        _normal = glm::normalize(normal);

        auto angle = glm::angle(getUp(), normal);
        auto axis = glm::cross(getUp(), normal);
        rotate(angle, axis);
    }

    mesh* plane::create(float width, float depth)
    {
        float halfWidth = width / 2.0f;
        float halfDepth = depth / 2.0f;

        glm::vec3 p0 = glm::vec3(-0.5f, 0.0f, -0.5f);
        glm::vec2 t0 = glm::vec2(0.0f, 0.0f);

        glm::vec3 p1 = glm::vec3(-0.5f, 0.0f, 0.5f);
        glm::vec2 t1 = glm::vec2(1.0f, 0.0f);

        glm::vec3 p2 = glm::vec3(0.5f, 0.0f, 0.5f);
        glm::vec2 t2 = glm::vec2(1.0f, 1.0f);

        glm::vec3 p3 = glm::vec3(0.5f, 0.0f, -0.5f);
        glm::vec2 t3 = glm::vec2(0.0f, 1.0f);

        glm::vec3 n = glm::vec3(0.0, 1.0, 0.0);
        glm::vec3 t = glm::vec3(1.0, 0.0, 0.0);

        std::vector<vertex> vertices;

        vertex v0 = vertex(p0, t0, n, t);
        vertex v1 = vertex(p1, t1, n, t);
        vertex v2 = vertex(p2, t2, n, t);
        vertex v3 = vertex(p3, t3, n, t);

        vertices.push_back(v0);
        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);

        auto indices = new std::vector<GLuint>();
        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);

        indices->push_back(2);
        indices->push_back(3);
        indices->push_back(0);

        return mesh::create("mesh0", vertices, indices);
    }

    void plane::setNormal(glm::vec3 normal)
    {
        _normal = glm::normalize(normal);

        auto angle = glm::angle(getUp(), normal);
        auto axis = glm::cross(getUp(), normal);
        rotate(angle, axis);
    }

    void plane::addPoint(glm::vec3 point)
    {
        _points.push_back(point);
    }

    void plane::update()
    {
        sceneObject::update();

        for (GLuint i = 0; i < _points.size(); i++)
        {
            glm::vec4 a = glm::vec4(_points[i].x, _points[i].y, _points[i].z, 0);
            glm::vec4 b = getModelMatrix() * a;
            _points[i] = glm::vec3(b.x, b.y, b.z);
        }
    }

    void plane::debugRender()
    {
        sceneObject::debugRender();

#ifdef WIN32
        //if (_points.size() == 0)
        //    return;

        //glPushMatrix();
        ////glDisable(GL_CULL_FACE);

        //glBegin(GL_LINE_STRIP);
        //glColor3f(getMaterial()->getDiffuseColor().r, getMaterial()->getDiffuseColor().g, getMaterial()->getDiffuseColor().b);

        //float width = getSize().width;
        //float depth = getSize().depth;

        ///*glm::vec3 botLeft = _position -_right * depth - _direction * width;
        //glm::vec3 botRight = _position -_right * depth + _direction * width;
        //glm::vec3 topLeft = _position + _right * depth - _direction * width;
        //glm::vec3 topRight = _position + _right * depth + _direction * width;*/

        //glm::vec3 botLeft = _points[0];
        //glm::vec3 botRight = _points[1];
        //glm::vec3 topRight = _points[2];
        //glm::vec3 topLeft = _points[3];

        //glVertex3f(botLeft.x, botLeft.y, botLeft.z); 
        //glVertex3f(botRight.x, botRight.y, botRight.z);
        //glVertex3f(topRight.x, topRight.y, topRight.z);
        //glVertex3f(topLeft.x, topLeft.y, topLeft.z);
        //glVertex3f(botLeft.x, botLeft.y, botLeft.z);
        //glEnd();
        //
        ////glEnable(GL_CULL_FACE);

        //glBegin(GL_LINES);
        //glColor3f(1.0f, 0.0f, 0.0f);
        //glVertex3f(_position.x + 0.01f, _position.y + 0.01f, _position.z + 0.01f); glVertex3f(_position.x  + 0.01f+ _right.x, _position.y  + 0.01f + _right.y, _position.z  + 0.01f + _right.z);
        //glColor3f(0.0f, 1.0f, 0.0f);
        //glVertex3f(_position.x + 0.01f, _position.y + 0.01f, _position.z + 0.01f); glVertex3f(_position.x  + 0.01f+ _up.x, _position.y  + 0.01f + _up.y, _position.z  + 0.01f + _up.z);
        //glColor3f(0.0f, 0.0f, 1.0f);
        //glVertex3f(_position.x + 0.01f, _position.y + 0.01f, _position.z + 0.01f); glVertex3f(_position.x  + 0.01f+ _direction.x, _position.y  + 0.01f + _direction.y, _position.z  + 0.01f + _direction.z);
        //
        //glEnd();

        //glPopMatrix();
#endif
    }
}
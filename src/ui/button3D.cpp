#include <phi/rendering/shaderManager.h>

#include <phi/ui/button3D.h>

namespace phi
{
    button3D::button3D(sizef viewportSize) :
        control(viewportSize)
    {
        _shader = shaderManager::get()->getShader("UI_MESH");
        _object = nullptr;
        createMesh();
    }

    void button3D::createMesh()
    {
        std::vector<vertex> vertices;
        auto indices = new std::vector<GLuint>();

        vec3 lbb = vec3(-0.5f, -0.5f, -0.5f);
        vec3 lbf = vec3(-0.5f, -0.5f, 0.5f);
        vec3 ltf = vec3(-0.5f, 0.5f, 0.5f);
        vec3 ltb = vec3(-0.5f, 0.5f, -0.5f);
        vec3 rbb = vec3(0.5f, -0.5f, -0.5f);
        vec3 rbf = vec3(0.5f, -0.5f, 0.5f);
        vec3 rtf = vec3(0.5f, 0.5f, 0.5f);
        vec3 rtb = vec3(0.5f, 0.5f, -0.5f);

        //             _________________________
        //            /|ltb                 rtb/|
        //           / |                      / |
        //          /  |                     /  |
        //         /   |                    /   |
        //        /    |                   /    |
        //       /     |                  /     |
        //      /      |                 /      |
        //     /       |                /       |
        //    /________________________/        |
        //    |ltf     |            rtf|        |
        //    |        |_______________|________|
        //    |       /lbb             |     rbb/
        //    |      /                 |       /
        //    |     /                  |      /
        //    |    /                   |     /
        //    |   /                    |    /
        //    |  /                     |   /
        //    | /                      |  /
        //    |/lbf                 rbf| /
        //    |________________________|/

        vertices.push_back(vertex(ltb, vec2(0.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f))); // 0
        vertices.push_back(vertex(lbb, vec2(0.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))); // 1
        vertices.push_back(vertex(ltf, vec2(1.0f, 1.0f), vec3(-1.0f, 0.0f, 0.0f))); // 2
        vertices.push_back(vertex(lbf, vec2(1.0f, 0.0f), vec3(-1.0f, 0.0f, 0.0f))); // 3

        vertices.push_back(vertex(ltb, vec2(0.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f))); // 4
        vertices.push_back(vertex(ltf, vec2(0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f))); // 5
        vertices.push_back(vertex(rtb, vec2(1.0f, 1.0f), vec3(0.0f, 1.0f, 0.0f))); // 6
        vertices.push_back(vertex(rtf, vec2(1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f))); // 7

        vertices.push_back(vertex(rtf, vec2(0.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f))); // 8
        vertices.push_back(vertex(rbf, vec2(0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f))); // 9
        vertices.push_back(vertex(rtb, vec2(1.0f, 1.0f), vec3(1.0f, 0.0f, 0.0f))); // 10
        vertices.push_back(vertex(rbb, vec2(1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f))); // 11

        vertices.push_back(vertex(lbf, vec2(0.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f))); // 12
        vertices.push_back(vertex(lbb, vec2(0.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f))); // 13
        vertices.push_back(vertex(rbf, vec2(1.0f, 1.0f), vec3(0.0f, -1.0f, 0.0f))); // 14
        vertices.push_back(vertex(rbb, vec2(1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f))); // 15

        vertices.push_back(vertex(ltf, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f))); // 16
        vertices.push_back(vertex(lbf, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f))); // 17
        vertices.push_back(vertex(rtf, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f))); // 18
        vertices.push_back(vertex(rbf, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f))); // 19

        vertices.push_back(vertex(rtb, vec2(0.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f))); // 20
        vertices.push_back(vertex(rbb, vec2(0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f))); // 21
        vertices.push_back(vertex(ltb, vec2(1.0f, 1.0f), vec3(0.0f, 0.0f, -1.0f))); // 22
        vertices.push_back(vertex(lbb, vec2(1.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f))); // 23

        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);
        indices->push_back(3);
        indices->push_back(2);
        indices->push_back(1);
        indices->push_back(4);
        indices->push_back(5);
        indices->push_back(6);
        indices->push_back(7);
        indices->push_back(6);
        indices->push_back(5);
        indices->push_back(8);
        indices->push_back(9);
        indices->push_back(10);
        indices->push_back(11);
        indices->push_back(10);
        indices->push_back(9);
        indices->push_back(12);
        indices->push_back(13);
        indices->push_back(14);
        indices->push_back(15);
        indices->push_back(14);
        indices->push_back(13);
        indices->push_back(16);
        indices->push_back(17);
        indices->push_back(18);
        indices->push_back(19);
        indices->push_back(18);
        indices->push_back(17);
        indices->push_back(20);
        indices->push_back(21);
        indices->push_back(22);
        indices->push_back(23);
        indices->push_back(22);
        indices->push_back(21);

        geometry::calcTangents(vertices, *indices);

        _geometry = geometry::create("button3D", vertices, indices);
    }

    void button3D::updateModelMatrix()
    {
        auto scale = glm::scale(vec3(0.2f, 0.2f, 0.03f));
        auto translation = translate(_object->getPosition());

        _modelMatrix = scale * translation;
    }

    void button3D::attachTo(object3D* object)
    {
        _object = object;
    }

    void button3D::onRender()
    {
       /* if (!_object)
            return;

        updateModelMatrix();
        auto mvp = _camera->getProjectionMatrix() * _camera->getViewMatrix() * _modelMatrix;

        _shader->bind();
        _shader->setUniform("mvp", mvp);
        _shader->setUniform("color", color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));
        _geometry->bind();
        _geometry->render();
        _geometry->unbind();
        _shader->unbind();*/
    }
}
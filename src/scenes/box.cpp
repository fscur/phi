#include <phi/scenes/box.h>
#include <phi/rendering/mesh.h>

namespace phi
{
    box::box(phi::size<float> size, phi::material* material)
        : sceneObject()
    {
        _model = new model("box", "");
        auto m = create(size);

        m->setMaterial(material);
        m->setId(0);
        _model->addMesh(m);
        auto sm = new sceneMesh(m);
        sm->setMaterial(material);
        _sceneMeshes.push_back(sm);

        _bulletShape = new btCompoundShape();
        _bulletShape->addChildShape(btTransform(), sm->getBulletShape());
        auto groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0.0f, 0.0f, 0.0f)));
        auto groundRigidBodyCI = btRigidBody::btRigidBodyConstructionInfo(0.0f, groundMotionState, _bulletShape, btVector3(0.0f, 0.0f, 0.0f));
        _bulletRigidBody = new btRigidBody(groundRigidBodyCI);
        _bulletRigidBody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
        _bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    }

    box::~box(void)
    {}

    mesh* box::create(phi::size<float> size)
    {
        std::vector<vertex> vertices;
        auto indices = new std::vector<GLuint>();

        auto hw = size.width * 0.5f;
        auto hh = size.height * 0.5f;
        auto hd = size.depth * 0.5f;

        glm::vec3 lbb = glm::vec3(-hw, -hh, -hd);
        glm::vec3 lbf = glm::vec3(-hw, -hh, hd);
        glm::vec3 ltf = glm::vec3(-hw, hh, hd);
        glm::vec3 ltb = glm::vec3(-hw, hh, -hd);
        glm::vec3 rbb = glm::vec3(hw, -hh, -hd);
        glm::vec3 rbf = glm::vec3(hw, -hh, hd);
        glm::vec3 rtf = glm::vec3(hw, hh, hd);
        glm::vec3 rtb = glm::vec3(hw, hh, -hd);

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

        vertices.push_back(vertex(ltb, glm::vec2(0.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f))); // 0
        vertices.push_back(vertex(lbb, glm::vec2(0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f))); // 1
        vertices.push_back(vertex(ltf, glm::vec2(1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f))); // 2
        vertices.push_back(vertex(lbf, glm::vec2(1.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f))); // 3

        vertices.push_back(vertex(ltb, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f))); // 4
        vertices.push_back(vertex(ltf, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))); // 5
        vertices.push_back(vertex(rtb, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f))); // 6
        vertices.push_back(vertex(rtf, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))); // 7

        vertices.push_back(vertex(rtf, glm::vec2(0.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f))); // 8
        vertices.push_back(vertex(rbf, glm::vec2(0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f))); // 9
        vertices.push_back(vertex(rtb, glm::vec2(1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f))); // 10
        vertices.push_back(vertex(rbb, glm::vec2(1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f))); // 11

        vertices.push_back(vertex(lbf, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); // 12
        vertices.push_back(vertex(lbb, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); // 13
        vertices.push_back(vertex(rbf, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f))); // 14
        vertices.push_back(vertex(rbb, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f))); // 15

        vertices.push_back(vertex(ltf, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))); // 16
        vertices.push_back(vertex(lbf, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); // 17
        vertices.push_back(vertex(rtf, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f))); // 18
        vertices.push_back(vertex(rbf, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))); // 19

        vertices.push_back(vertex(rtb, glm::vec2(0.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f))); // 20
        vertices.push_back(vertex(rbb, glm::vec2(0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))); // 21
        vertices.push_back(vertex(ltb, glm::vec2(1.0f, 1.0f), glm::vec3(0.0f, 0.0f, -1.0f))); // 22
        vertices.push_back(vertex(lbb, glm::vec2(1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f))); // 23

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

        mesh::calcTangents(vertices, *indices);

        return mesh::create("box", vertices, indices);
    }
}
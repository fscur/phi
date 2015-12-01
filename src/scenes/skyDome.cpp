#include <phi/core/mathUtils.h>

#include <phi/scenes/skyDome.h>

namespace phi
{
    float SEC_IN_RADIANS = 0.00007272f;
    float T = 0.04166667f;

    skyDome::skyDome(float radius, int rings, int sectors, material* material)
        : sceneObject()
    {
        _radius = radius;

        auto d = 2.0f * radius;
        setSize(size<float>(d, radius, d));

        _model = new model("skyDome", "");
        auto m = create(rings, sectors);
        m->setMaterial(material);
        _model->addMesh(m);

        _sun = new phi::directionalLight(glm::vec3(), phi::color::fromRGBA(1.0f, 0.9f, 0.6f, 1.0f), 1.0f, glm::vec3());
    }

    skyDome::~skyDome()
    {
    }

    mesh* skyDome::create(GLuint rings, GLuint sectors)
    {
        float piOver2 = PI * 0.5f;

        std::vector<vertex> vertices;
        auto indices = new std::vector<GLuint>();

        float const R = 1.0f / (float)(rings - 1.0f);
        float const S = 1.0f / (float)(sectors - 1.0f);
        GLuint r, s;

        for (r = 0; r < rings; r++)
        {
            for (s = 0; s < sectors; s++)
            {
                float const y = glm::sin(-piOver2 + PI * r * R) * 0.5f;
                float const x = glm::cos(2.0f * PI * s * S) * glm::sin(PI * r * R) * 0.5f;
                float const z = glm::sin(2.0f * PI * s * S) * glm::sin(PI * r * R) * 0.5f;

                glm::vec3 position = glm::vec3(x, y, z);
                glm::vec3 normal = -glm::normalize(glm::vec3(x, y, z));
                glm::vec2 texCoord = glm::vec2(s * R, r * R);
                vertices.push_back(vertex(position, texCoord, normal));
            }
        }

        for (r = 0; r < rings - 1; r++)
        {
            for (s = 0; s < sectors - 1; s++)
            {
                int a = r * sectors + s;
                int b = (r + 1) * sectors + s;
                int c = (r + 1) * sectors + (s + 1);
                int d = r * sectors + (s + 1);

                indices->push_back(a);
                indices->push_back(b);
                indices->push_back(c);
                indices->push_back(c);
                indices->push_back(d);
                indices->push_back(a);
            }
        }

        mesh::calcTangents(vertices, *indices);

        return mesh::create("skyDome", vertices, indices);
    }

    float skyDome::getLightChanellValue2(float a, float b, float c, float d, float e)
    {
        float t0 = (2.0f * e) - 1.0f; //-1 -> 0 -> 1
        float f = pow(abs(t0), d);
        return (1.0f / ((c * f) + 1.0f)) * a + b;
    }

    phi::color skyDome::calcHorizonColor(float t)
    {
        float r = getLightChanellValue2(0.9f, 0.1f, 8000.0f, 16.0f, t * T);
        float g = getLightChanellValue2(0.9f, 0.1f, 9000.0f, 12.0f, t * T);
        float b = getLightChanellValue2(0.8f, 0.2f, 1000.0f, 8.0f, t * T);

        return phi::color(glm::clamp<float>(r, 0.0f, 1.0f), glm::clamp<float>(g, 0.0f, 1.0f), glm::clamp<float>(b, 0.0f, 1.0f), 1.0f);
    }

    void skyDome::updateSun()
    {
        float alpha = ((_dayTime * 3600) - 21600) * SEC_IN_RADIANS;
        float beta = glm::radians(_sunOrientation);

        float x = glm::cos(beta) * glm::cos(alpha);
        float y = glm::sin(alpha);
        float z = -glm::sin(beta) * glm::cos(alpha);

        glm::vec3 pos = glm::vec3(x, y, z) * _radius;
        glm::vec3 dir = -glm::normalize(pos);

        _sun->setLocalPosition(pos);
        _sun->setDirection(dir);

        _horizonColor = calcHorizonColor(_dayTime);
        _sun->setColor(_horizonColor);
    }

    void skyDome::setRadius(float radius)
    {
        _radius = radius;
        setSize(size<float>(radius, radius, radius));
    }

    void skyDome::setDayTime(float value)
    {
        _dayTime = value;
        updateSun();
    }

    void skyDome::setSunOrientation(float value)
    {
        _sunOrientation = value;
        updateSun();
    }

    void skyDome::render()
    {
    }
}
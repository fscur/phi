#include "cone.h"

namespace phi
{
    cone::cone(float height, float radius, unsigned int sectors, material* material)
        : sceneObject()
    {   
        _radius = radius;
        auto d =  2.0f * radius;
        setSize(size<float>(d, d, height));

        _model = new model("cone", "");
        auto m = create(sectors);
        m->setMaterial(material);
        _model->addMesh(m);

        _model->addMesh(m);
    }

    cone::~cone()
    {
    }

    mesh* cone::create(unsigned int sectors)
    {
        //TODO: bug-> When creating a cone with 100 sides or more, the geometry is fucked up!!!!!
        std::vector<vertex> tempVertices;
        std::vector<GLuint> indices;

        glm::vec3 top = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 bot = glm::vec3(0.0f, 0.0f, 1.0f);

        float radius = 0.5f;
        int t = sectors;
        int verts = 6;

        double increment = 1.0f/((double)t);

        for (int i = 0; i < t * verts; i += verts)
        {
            double u = increment * (double)i/(double)verts;
            double v = u + increment;

            double angle0 = u * PI * 2.0;
            double angle1 = v * PI * 2.0;

            double x0 = cos(-angle0) * radius;
            double y0 = sin(-angle0) * radius;
            double x1 = cos(-angle1) * radius;
            double y1 = sin(-angle1) * radius;

            glm::vec3 a = glm::vec3(x0, y0, 1.0f);
            glm::vec3 b = glm::vec3(x1, y1, 1.0f);

            glm::vec3 v0 = a - top;
            glm::vec3 v1 = b - top;

            glm::vec3 normal = glm::normalize(glm::cross(v0, v1));

            tempVertices.push_back(vertex(top, glm::vec2((u+v)/2.0f, 1.0f), normal));
            tempVertices.push_back(vertex(a, glm::vec2(u, 0.0f), normal));
            tempVertices.push_back(vertex(b, glm::vec2(v, 0.0f), normal));

            a = glm::vec3(x1, y1, 1.0f);
            b = glm::vec3(x0, y0, 1.0f);

            v0 = a - bot;
            v1 = b - bot;

            normal = glm::normalize(glm::cross(v0, v1));

            tempVertices.push_back(vertex(bot, glm::vec2(0.5f, 0.5f), normal));
            tempVertices.push_back(vertex(a, glm::vec2(u, 0.0f), normal));
            tempVertices.push_back(vertex(b, glm::vec2(v, 0.0f), normal));
        }

        std::vector<glm::vec3> normals;

        for (int i = 0; i < t * verts; i += verts)
        {
            int j = i == 0 ? (t - 1) * verts : i - verts;

            vertex* v0 = &tempVertices[i];
            vertex* v1 = &tempVertices[j];

            glm::vec3 n0 = v0->getNormal();
            glm::vec3 n1 = v1->getNormal();
            glm::vec3 n = glm::normalize(n0 + n1);

            normals.push_back(n);
        }

        for (int i = 0; i < t * verts; i += verts)
        {
            int j = i == 0 ? (t - 1) * verts : i - verts;
            int k = i/verts;

            vertex* v = &tempVertices[i];
            v->setNormal(normals[k]);

            v = &tempVertices[i+1];
            v->setNormal(normals[k]);

            v = &tempVertices[j+2];
            v->setNormal(normals[k]);
        }

        //select vertices and create indices
        std::vector<vertex> vertices;
        unsigned int vSize = tempVertices.size();

        vertex vertex;
        bool exists = false;
        int index = 0;
        unsigned int i = 0;
        unsigned int j = 0;
        unsigned int verticesSize = 0;

        for(i = 0; i < vSize; i++)
        {
            vertex = tempVertices[i];
            exists = false;
            index = 0;

            for(j = 0; j < verticesSize; j++)
            {
                if (vertices[j] == vertex)
                {
                    exists = true;
                    break;
                }

                index++;
            }

            if (!exists)
            {
                index = verticesSize++;
                vertices.push_back(vertex);
            }

            indices.push_back(index);
        }

        return mesh::create("cone", vertices, indices);
    }

    void cone::setRadius(float radius)
    { 
        _radius = radius;
        setSize(size<float>(_radius, _radius, _height));
    }

    void cone::setHeight(float height)
    { 
        _height = height;
        setSize(size<float>(_radius, _height, _radius));
    }

    void cone::debugRender()
    {
#ifdef WIN32
        /*
        std::vector<vertex> vertices = _mesh->getVertices();

        glPushMatrix();
        //glDisable(GL_CULL_FACE);

        glBegin(GL_LINES);
        color color = color::red;

        glColor3f(color.r, color.g, color.b);

        for (unsigned int i = 0; i < vertices.size(); i++)
        {
        glm::vec3 a = vertices[i].getPosition();
        glm::vec3 b = a + vertices[i].getNormal();

        glVertex3f(a.x, a.y, a.z); 
        glVertex3f(b.x, b.y, b.z);
        }

        /*color = Color::White;
        glColor3f(color.R, color.G, color.B);*/

        /*for (int index = 0; index < vertices.size(); index += 6)
        {
        int topIndex = index;
        int i = index + 1;
        int j = index + 2;
        int k = index + 1;
        int l = index + 2;
        int botIndex = index + 5;

        glm::vec3 a = vertices[topIndex].GetPosition();
        glm::vec3 b = vertices[i].GetPosition();
        glm::vec3 c = vertices[j].GetPosition();

        glVertex3f(a.x, a.y, a.z); 
        glVertex3f(b.x, b.y, b.z);

        glVertex3f(b.x, b.y, b.z); 
        glVertex3f(c.x, c.y, c.z);

        glVertex3f(c.x, c.y, c.z);
        glVertex3f(a.x, a.y, a.z);

        a = vertices[botIndex].GetPosition();
        b = vertices[k].GetPosition();
        c = vertices[l].GetPosition();

        glVertex3f(a.x, a.y, a.z); 
        glVertex3f(b.x, b.y, b.z);

        glVertex3f(b.x, b.y, b.z); 
        glVertex3f(c.x, c.y, c.z);

        glVertex3f(c.x, c.y, c.z);
        glVertex3f(a.x, a.y, a.z);
        }*/

        //glEnd();

        //glPopMatrix();
#endif

    }
}
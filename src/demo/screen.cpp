#include <phi\demo\screen.h>
#include <phi\diagnostics\diagnostics.h>
#include <phi\diagnostics\stopwatch.h>
#include <phi\core\globals.h>
#include <phi\rendering\model.h>

#include <phi\rendering\shaderManager.h>
#include <GLM\gtc\constants.hpp>

screen::screen() : form()
{
    _temp = 0.0f;
    _isMouseDown = false;
    _rotating = false;
    _shadowMap = false;
    _translationSpeed = 1.0f;
    _rotationSpeed = 0.01f;
    _sceneRenderer = nullptr;
}

screen::~screen()
{
}

void screen::initScene()
{
    _scene = new phi::scene(new phi::camera(0.1f, 1000.0f, getSize(), glm::half_pi<float>()));
    auto camera = _scene->getCamera();

    camera->setLocalPosition(phi::vec3(5.0f, -5.0f, 5.0f));
    camera->setTarget(phi::vec3(0.0f, 0.0f, 0.0f));
    camera->update();

    auto newTriangle = [](phi::vertex A, phi::vertex B, phi::vertex C)
    {
        auto vertices = std::vector<phi::vertex>();

        vertices.push_back(A);
        vertices.push_back(B);
        vertices.push_back(C);

        auto indices = new std::vector<uint>();
        indices->push_back(0);
        indices->push_back(1);
        indices->push_back(2);

        auto gd = phi::geometryData::create(vertices, indices);
        return new phi::geometry(gd);
    };

    auto text = phi::texture::fromFile("C:\\test.bmp");
    auto textureIdx = text->getId();

    //GLuint64 handle = glGetTextureHandleARB(textureIdx);
    //glMakeTextureHandleResidentARB(handle);

    auto red = phi::material::getLambert(phi::color::red);
    auto blue = phi::material::getLambert(phi::color::blue);
    auto textureMat = new phi::material(
        "tex",
        "",
        phi::texture::fromFile("c:\\test.bmp"),
        phi::texture::getDefaultNormal(),
        phi::texture::getDefaultSpecular(),
        phi::texture::getDefaultEmissive(),
        phi::color::white,
        phi::color::white,
        phi::color::white,
        phi::color::white,
        0.2f,
        0.8f,
        0.0f,
        0.0f,
        0.0f,
        false,
        nullptr);

    //textureMat->setDiffuseHandle(handle);

    auto v0 = phi::vec3(+0.5f, +0.5f, +0.5f);
    auto v1 = phi::vec3(+0.5f, +0.5f, -0.5f);
    auto v2 = phi::vec3(+0.5f, -0.5f, +0.5f);
    auto v3 = phi::vec3(+0.5f, -0.5f, -0.5f);
    auto v4 = phi::vec3(-0.5f, +0.5f, +0.5f);
    auto v5 = phi::vec3(-0.5f, +0.5f, -0.5f);
    auto v6 = phi::vec3(-0.5f, -0.5f, +0.5f);
    auto v7 = phi::vec3(-0.5f, -0.5f, -0.5f);

    auto t0 = phi::vec2(0.0f, 0.0f);
    auto t1 = phi::vec2(1.0f, 0.0f);
    auto t2 = phi::vec2(1.0f, 1.0f);
    auto t3 = phi::vec2(0.0f, 1.0f);

    auto n0 = phi::vec3(+1.0f, +0.0f, +0.0f);
    auto n1 = phi::vec3(+0.0f, +1.0f, +0.0f);
    auto n2 = phi::vec3(+0.0f, +0.0f, +1.0f);
    auto n3 = phi::vec3(-1.0f, +0.0f, +0.0f);
    auto n4 = phi::vec3(+0.0f, -1.0f, +0.0f);
    auto n5 = phi::vec3(+0.0f, +0.0f, -1.0f);

    auto tri0 = newTriangle(phi::vertex(v3, t0, n5), phi::vertex(v7, t1, n5), phi::vertex(v5, t2, n5));
    auto tri1 = newTriangle(phi::vertex(v5, t2, n5), phi::vertex(v1, t3, n5), phi::vertex(v3, t0, n5));
    auto tri2 = newTriangle(phi::vertex(v7, t0, n3), phi::vertex(v6, t1, n3), phi::vertex(v4, t2, n3));
    auto tri3 = newTriangle(phi::vertex(v4, t2, n3), phi::vertex(v5, t3, n3), phi::vertex(v7, t0, n3));
    auto tri4 = newTriangle(phi::vertex(v6, t0, n2), phi::vertex(v2, t1, n2), phi::vertex(v0, t2, n2));
    auto tri5 = newTriangle(phi::vertex(v0, t2, n2), phi::vertex(v4, t3, n2), phi::vertex(v6, t0, n2));
    auto tri6 = newTriangle(phi::vertex(v2, t0, n0), phi::vertex(v3, t1, n0), phi::vertex(v1, t2, n0));
    auto tri7 = newTriangle(phi::vertex(v1, t2, n0), phi::vertex(v0, t3, n0), phi::vertex(v2, t0, n0));
    auto tri8 = newTriangle(phi::vertex(v4, t0, n2), phi::vertex(v0, t1, n2), phi::vertex(v1, t2, n2));
    auto tri9 = newTriangle(phi::vertex(v1, t2, n2), phi::vertex(v5, t3, n2), phi::vertex(v4, t0, n2));
    auto tri10 = newTriangle(phi::vertex(v7, t0, n0), phi::vertex(v3, t1, n0), phi::vertex(v2, t2, n0));
    auto tri11 = newTriangle(phi::vertex(v2, t2, n0), phi::vertex(v6, t3, n0), phi::vertex(v7, t0, n0));

    auto n = 10;

    for (auto x = 0; x < 10; x++)
    {
        for (auto y = 0; y < 10; y++)
        {
            for (auto z = 0; z < 10; z++)
            {
                auto face0 = new phi::model("face0");
                face0->addChild(new phi::mesh("tri0", tri0, textureMat));
                face0->addChild(new phi::mesh("tri1", tri1, textureMat));

                auto face1 = new phi::model("face1");
                face1->addChild(new phi::mesh("tri2", tri2, textureMat));
                face1->addChild(new phi::mesh("tri3", tri3, textureMat));

                auto face2 = new phi::model("face2");
                face2->addChild(new phi::mesh("tri4", tri4, textureMat));
                face2->addChild(new phi::mesh("tri5", tri5, textureMat));

                auto face3 = new phi::model("face3");
                face3->addChild(new phi::mesh("tri6", tri6, textureMat));
                face3->addChild(new phi::mesh("tri7", tri7, textureMat));

                auto face4 = new phi::model("face4");
                face4->addChild(new phi::mesh("tri8", tri8, textureMat));
                face4->addChild(new phi::mesh("tri9", tri9, textureMat));

                auto face5 = new phi::model("face5");
                face5->addChild(new phi::mesh("tri10", tri10, textureMat));
                face5->addChild(new phi::mesh("tri11", tri11, textureMat));

                auto model = new phi::model("cube");
                model->addChild(face0);
                model->addChild(face1);
                model->addChild(face2);
                model->addChild(face3);
                model->addChild(face4);
                model->addChild(face5);

                _scene->add(model);

                auto faces = model->getChildren();
                auto facesSize = faces.size();

                for (auto i = 0; i < facesSize; i++)
                {
                    auto face = faces[i];

                    auto meshes = face->getChildren();
                    auto meshesSize = meshes.size();

                    for (auto j = 0; j < meshesSize; j++)
                    {
                        auto mesh = static_cast<phi::mesh*>(meshes[j]);

                        mesh->setLocalPosition(phi::vec3(x, y, z));
                        mesh->update();
                    }
                }
            }
        }
    }

    auto info = phi::shaderManagerInfo();
    info.path = _resourcesPath;
    phi::shaderManager::get()->init(info);

    _sceneRenderer = new phi::sceneRenderer();
    _sceneRenderer->init();
}

void screen::onInitialize()
{
    setTitle("Teste");
    centerScreen();
    initScene();
}

float t = 0.0f;

void screen::update()
{
    t += 0.1f;
    _scene->getCamera()->setLocalPosition(phi::vec3(glm::cos(t), -1.0f, glm::sin(t)) * 5.0f);
    _scene->getCamera()->update();
    _scene->update();
}

void screen::render()
{
    _sceneRenderer->render(_scene);
}

void screen::onResize(SDL_Event e)
{
    auto sz = phi::sizef((float)e.window.data1, (float)e.window.data2);
    setSize(sz);
    _scene->getCamera()->setResolution(sz);
}

void screen::onClosing()
{
    //TODO: MessageBox asking if the user really wants to close the window
}

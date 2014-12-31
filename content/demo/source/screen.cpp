#include "screen.h"
#include "box.h"
#include "plane.h"
#include "sphere.h"
#include "cone.h"
#include "colorAnimator.h"
#include "fsSceneRenderer.h"
#include "dsSceneRenderer.h"

screen::screen() : form()
{
	_temp = 0.0f;
	_isMouseDown = false;
	_rotating = false;
	_shadowMap = false;
	_translationSpeed = 1.0f;
	_rotationSpeed = 0.01f;
}

screen::~screen()
{
	phi::engine::get()->release();
}

void screen::initEngine()
{
	phi::engineInfo info;
	info.applicationPath = getApplicationPath();
	info.size = getSize();

	phi::engine::get()->init(info);
 }

void screen::initScene()
{
	phi::scene* s = new phi::scene();
	s->setSize(getSize());
	s->getActiveCamera()->setPosition(glm::vec3(5.0f, 5.0f, 5.0f));
	s->getActiveCamera()->setTarget(glm::vec3(0.0f, 0.0f, 0.0f));
	s->setAmbientLightColor(phi::color::fromRGBA(1.0f, 1.0f, 1.0f, 1.0f));

	box* box1 = new box(glm::vec3(), phi::size<float>(1.0f, 1.0f, 1.0f), phi::rendering::repository->getResource<phi::material>("bricks"));
	box1->setPosition(glm::vec3(0.0f, 0.5f, 0.0f));
	s->add(box1);

	phi::plane* floor = new phi::plane(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), phi::size<float>(1000.0f, 0.1f, 1000.0f), phi::rendering::repository->getResource<phi::material>("white"));
	s->add(floor);

	phi::directionalLight* dirLight = new phi::directionalLight(glm::vec3(0.0f, 2.0f, 2.0f), phi::color::white, 1.0f, glm::vec3(-0.4f, -1.0f, -0.7f));
	s->add(dirLight);

	phi::pointLight* pLight = new phi::pointLight(glm::vec3(0.0f, 5.0f, 0.0f), phi::color::blue, 10.0f, phi::attenuation());
	s->add(pLight);

	phi::spotLight* sLight = new phi::spotLight(glm::vec3(0.0f, 5.0f, 0.0f), phi::color::orange, 100.0f, phi::attenuation(), glm::vec3(0.0f, -1.0f, 0.0f), 0.8f);
	s->add(sLight);

	phi::engine::get()->addScene(s);
	phi::engine::get()->loadScene(0);
}

void screen::initUI()
{
    phi::ui::init(getApplicationPath());

	phi::color labelBackground = phi::color::transparent;
	phi::color labelForeground = phi::color::white;

	_hud = new phi::hud();

	phi::button* closeButton = new phi::button(getSize());
	closeButton->setText("X");
	closeButton->setBackgroundColor(phi::color::red);
	closeButton->setForegroundColor(phi::color::white);
	closeButton->setSize(phi::size<GLuint>(24, 24));
	closeButton->setX(1024 - 28);
	closeButton->setY(4);
	closeButton->getClick()->bind<screen, &screen::closeButtonClick>(this);
	_hud->addControl(closeButton);

	phi::button* expandButton = new phi::button(getSize());
	expandButton->setText("Texto");
	expandButton->setBackgroundColor(phi::color::blue);
	expandButton->setForegroundColor(phi::color::white);
	expandButton->setSize(phi::size<GLuint>(48, 48));
	expandButton->setX(10);
	expandButton->setY(10);
	expandButton->getClick()->bind<screen, &screen::expandButtonClick>(this);
	_hud->addControl(expandButton);

	_labelFps = new phi::label(getSize());
	_labelFps->setX(68);
	_labelFps->setY(10);
	_labelFps->setSize(phi::size<GLuint>(150, 20));
	_labelFps->setForegroundColor(labelForeground);
	_labelFps->setBackgroundColor(labelBackground);
	_labelFps->setFont(phi::ui::repository->getResource<phi::font>("Consola_24"));
	_hud->addControl(_labelFps);

	_labelObjects= new phi::label(getSize());
	_labelObjects->setX(68);
	_labelObjects->setY(28);
	_labelObjects->setSize(phi::size<GLuint>(150, 20));
	_labelObjects->setForegroundColor(labelForeground);
	_labelObjects->setBackgroundColor(labelBackground);
	_hud->addControl(_labelObjects);

	_labelDt = new phi::label(getSize());
	_labelDt->setX(220);
	_labelDt->setY(10);
	_labelDt->setSize(phi::size<GLuint>(300, 20));
	_labelDt->setForegroundColor(labelBackground);
	_labelDt->setBackgroundColor(labelBackground);
	_hud->addControl(_labelDt);

	_labelInputCost = new phi::label(getSize());
	_labelInputCost->setX(220);
	_labelInputCost->setY(28);
	_labelInputCost->setSize(phi::size<GLuint>(300, 20));
	_labelInputCost->setForegroundColor(labelForeground);
	_labelInputCost->setBackgroundColor(labelBackground);
	_hud->addControl(_labelInputCost);

	_labelUpdateCost = new phi::label(getSize());
	_labelUpdateCost->setX(220);
	_labelUpdateCost->setY(46);
	_labelUpdateCost->setSize(phi::size<GLuint>(300, 20));
	_labelUpdateCost->setForegroundColor(labelForeground);
	_labelUpdateCost->setBackgroundColor(labelBackground);
	_hud->addControl(_labelUpdateCost);

	_labelRenderCost = new phi::label(getSize());
	_labelRenderCost->setX(220);
	_labelRenderCost->setY(64);
	_labelRenderCost->setSize(phi::size<GLuint>(300, 20));
	_labelRenderCost->setForegroundColor(labelForeground);
	_labelRenderCost->setBackgroundColor(labelBackground);
	_hud->addControl(_labelRenderCost);

	_slider1 = new phi::slider(getSize());
	_slider1->setX(20);
	_slider1->setY(200);
	_slider1->setSize(phi::size<GLuint>(500, 30));
	_slider1->setSliderColor(phi::color::gray);
	_slider1->setTrackColor(phi::color::white);
	_slider1->setMinValue(0);
	_slider1->setMaxValue(200);
	_slider1->setStepValue(0.1f);
	_slider1->getValueChanged()->bind<screen, &screen::sliderValueChanged>(this);
	_hud->addControl(_slider1);
}

//void Screen::CloseButtonMouseEnter(MouseEventArgs e)
//{
//    _closeButton->SetBackColor(Color::White);
//}

//void Screen::CloseButtonMouseLeave(MouseEventArgs e)
//{
//    _closeButton->SetBackColor(Color::Gray);
//}

//void Screen::Update()
//{
//	Scene* scene = Engine::Get()->GetScene();
//
//    ////LOG(dt);
//    _temp += 0.01;
//
//    float sine = (float)sin(_temp);
//    float cossine = (float)cos(_temp);
//
//    PointLight* light0 = dynamic_cast<PointLight*>(scene->GetLight(1));
//	vec3 position0 = light0->GetPosition();
//    light0->SetPosition(vec3(cossine * 4.0f, position0.y, sine * 4.0f));
//	
//	/*PointLight* light1 = dynamic_cast<PointLight*>(scene->GetLight(2));
//	vec3 position1 = light1->GetPosition();
//    light1->SetPosition(vec3(-cossine * 4.0f, position1.y, sine * 4.0f));
//
//	SpotLight* light2 = dynamic_cast<SpotLight*>(scene->GetLight(3));
//    vec3 direction = light2->GetDirection();
//    light2->SetDirection(vec3(cossine, -abs(sine), direction.z));*/
//
//	//DirectionalLight* light = dynamic_cast<DirectionalLight*>(scene->GetLight(0));
//    //light->SetDirection(vec3(cossine, -1.0f, sine));
//
//    //light = dynamic_cast<PointLight*>(Globals::GetAt<Light>(_lights, 2));
//    //light->SetPosition(vec3(0.0f, -cossine, sine));
//
//    //light = dynamic_cast<PointLight*>(Globals::GetAt<Light>(_lights, 3));
//    //light->SetPosition(vec3(cossine, -sine, 0.0));
//
//    /*SceneObject* box =  scene->GetSceneObject(0);
//    box->Pitch(0.01);*/
//
//    Engine::Get()->Update();
//
//    /*_txtFps->SetText("FPS: " + to_string(GetFps()));
//    _txtDt->SetText("TotalTime/Frame (s): " + to_string(GetDt()));
//    _txtInputCost->SetText("InputCost (s): " + to_string(GetInputCost()));
//    _txtUpdateCost->SetText("UpdateCost (s): " + to_string(GetUpdateCost()));
//    _txtRenderCost->SetText("RenderCost (s): " + to_string(GetRenderCost()));*/
//
//    int visibleObjects = Engine::Get()->GetScene()->GetVisibleObjectsCount();
//    int allObjects = Engine::Get()->GetScene()->GetAllObjectsCount();
//
//    //_txtObjects->SetText(to_string(visibleObjects) + "/" + to_string(allObjects));
//}

void screen::initialize(std::string applicationPath)
{
	form::initialize(applicationPath);
	
	setTitle("Teste");
	setSize(phi::size<unsigned int>(1024, 768));
	centerScreen();

	initEngine();

	initScene();
	initUI();
}

void screen::onResize(SDL_Event e)
{
	phi::size<GLuint> sz = phi::size<GLuint>(e.window.data1, e.window.data2);
	phi::engine::get()->resize(sz);
	_hud->setViewportSize(sz);
}

void screen::onMouseDown(SDL_Event e)
{
	bool handled = false;

	//handled = _hud->HandleMouseDown(e);

	if (handled)
		return;

	_isMouseDown = true;
	_mouseDownPos = glm::vec2(e.motion.x, e.motion.y);
}

void screen::onMouseMove(SDL_Event e) 
{
	glm::vec2 mousePos = glm::vec2(e.motion.x, e.motion.y);

	bool handled = false;

	//handled = _hud->HandleMouseMove(e);

	if (handled)
		return;

	if (!_isMouseDown)
	{
		_lastMousePos = mousePos;
		return;
	}

	if (_isMouseDown && glm::length(_mouseDownPos - _lastMousePos) > 5)
		_rotating = true;

	if (!_rotating)
	{
		_lastMousePos = mousePos;
		return;
	}

	float dx = (float)(_lastMousePos.x - mousePos.x);
	float dy = (float)(_lastMousePos.y - mousePos.y);

	glm::vec2 displacement = glm::vec2(dx, dy);

	phi::camera* camera = phi::engine::get()->getScene()->getActiveCamera();

	dx *= _rotationSpeed;
	dy *= _rotationSpeed;

	camera->orbit(camera->getTarget(), camera->getUp(), dx);
	camera->orbit(camera->getTarget(), camera->getRight(), dy);

	_lastMousePos = mousePos;
}

void screen::onMouseUp(SDL_Event e)
{
	bool handled = false;

	//handled = _hud->HandleMouseUp(e);

	if (handled)
		return;

	if (!_rotating)
	{
		// Return the depth buffer value at (x,y)
		//glPixelTransferf( GL_DEPTH_SCALE, 1.0f );
		//glPixelTransferf( GL_DEPTH_BIAS, 0.0f );
		//float z = 1;    // default to far clipping plane
		//// glReadPixels returns the depth buffer value mapping the current
		//// glDepthRange near-to-far to 0.0-1.0
		//glReadPixels((GLuint)_lastMousePos.x, GetHeight() - (GLuint)_lastMousePos.y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z );

		glm::vec2 screenCoords = glm::vec2(e.motion.x, e.motion.y);
		phi::size<float> screenSize = phi::size<float>((float)getSize().width, (float)getSize().height);
		_ray = phi::engine::get()->getScene()->getActiveCamera()->castRay(screenCoords, screenSize);
		std::vector<phi::sceneObject*> sceneObjects = phi::engine::get()->getScene()->getVisibleObjects();

		/*for (unsigned int i = 0; i < sceneObjects.size(); i++)
		{
		SceneObject* sceneObject = sceneObjects[i];

		sceneObject->SetSelected(false);

		if (_ray.Intersects(sceneObject->GetAABB()))
		sceneObject->SetSelected(true);
		}*/
	}

	_rotating = false;
	_isMouseDown = false;
}

void screen::onMouseWheel(SDL_Event e)
{
	phi::camera* camera = phi::engine::get()->getScene()->getActiveCamera();

	if (e.wheel.y > 0)
		camera->zoomIn();
	else
		camera->zoomOut();
}

void screen::onKeyDown(SDL_Event e)
{
	form::onKeyDown(e);

	if (e.key.keysym.sym == SDLK_ESCAPE)
		close();
}

void screen::onKeyUp(SDL_Event e)
{
	form::onKeyUp(e);
}

void screen::onClosing()
{
	//TODO: MessageBox asking if the user really wants to close the window
}

void screen::expandButtonClick(phi::mouseEventArgs e)
{
	/*if(_shadowMap)
	_tempCamera = Engine::Get()->GetScene()->GetActiveCamera();
	else
	_tempCamera = _debugCamera;*/

	_shadowMap = !_shadowMap;
}

void screen::sliderValueChanged(phi::eventArgs e)
{
	_labelFps->setText(std::to_string(_slider1->getValue()));
}

void screen::closeButtonClick(phi::mouseEventArgs e)
{
	close();
}

float a;

void screen::update()
{
	a+= 0.001;
	if (a > 2 * phi::PI)
		a -=2 * phi::PI;

	phi::engine::get()->update();
	phi::colorAnimator::update();

	_labelFps->setText("FPS: " + std::to_string(getFps()));
	_labelDt->setText("TotalTime/Frame (s): " + std::to_string(getDt()));
	_labelInputCost->setText("InputCost (s): " + std::to_string(getInputCost()));
	_labelUpdateCost->setText("UpdateCost (s): " + std::to_string(getUpdateCost()));
	_labelRenderCost->setText("RenderCost (s): " + std::to_string(getRenderCost()));

	//int visibleObjects = Engine::Get()->GetScene()->GetVisibleObjectsCount();
	//int allObjects = Engine::Get()->GetScene()->GetAllObjectsCount();
	//_labelObjects->setText(to_string(visibleObjects) + "/" + to_string(allObjects));

	phi::pointLight* p = dynamic_cast<phi::pointLight*>(phi::engine::get()->getScene()->getPointLight(0));
	p->setPosition(glm::vec3(glm::cos(a) * 5.0f, 0.5f, glm::sin(a) * 3.5f));

	phi::spotLight* s = dynamic_cast<phi::spotLight*>(phi::engine::get()->getScene()->getSpotLight(0));
	auto dir = s->getDirection();
	s->setDirection(glm::vec3(glm::cos(a) * 3.5f, -abs(glm::sin(a) * 3.5f), dir.z));
}

void screen::render()
{
	phi::engine::get()->render();
	_hud->render();
}
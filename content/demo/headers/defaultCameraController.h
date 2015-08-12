#ifndef _DEFAULT_CAMERA_CONTROLLER_H_
#define _DEFAULT_CAMERA_CONTROLLER_H_

#include "cameraController.h"
#include "shortcuts.h"
#include <glm/glm.hpp>
#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

class defaultCameraController :
    public cameraController
{
private:
    shortcuts _shortcuts;
    glm::vec2 _lastMousePos;
    bool _rotating;
    bool _panning;
    GLfloat _zBufferValue;
    glm::vec3 _cameraPos;
    glm::vec3 _cameraRight;
    glm::vec3 _cameraUp;
    glm::vec2 _startPos;
    float _eyeZ;
    glm::vec3 _targetPos;

private:
    void executeInput(phi::inputKey key, glm::vec2 mousePos);
    void initPan(glm::vec2 mousePos);
    void initRotate(glm::vec2 mousePos);
    void zoom(glm::vec2 mousePos, bool in);
    void pan(glm::vec2 mousePos);
    void rotate(glm::vec2 mousePos);

public:
    defaultCameraController(phi::size<unsigned int> viewportSize);

    virtual bool onMouseDown(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onMouseMove(phi::mouseEventArgs* e) override;
    virtual bool onMouseUp(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key) override;

    virtual void update() override;
};

#endif
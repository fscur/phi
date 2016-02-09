#ifndef _DEFAULT_CAMERA_CONTROLLER_H_
#define _DEFAULT_CAMERA_CONTROLLER_H_

#include <phi/core/globals.h>
#include "cameraController.h"
#include "shortcuts.h"

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
    phi::vec2 _lastMousePos;
    bool _rotating;
    bool _panning;
    GLfloat _zBufferValue;
    phi::vec3 _cameraPos;
    phi::vec3 _cameraRight;
    phi::vec3 _cameraUp;
    phi::vec2 _startPos;
    float _eyeZ;
    phi::vec3 _targetPos;

private:
    void executeInput(phi::inputKey key, phi::vec2 mousePos);
    void initPan(phi::vec2 mousePos);
    void initRotate(phi::vec2 mousePos);
    void zoom(phi::vec2 mousePos, bool in);
    void pan(phi::vec2 mousePos);
    void rotate(phi::vec2 mousePos);

public:
    defaultCameraController(phi::camera* camera);

    virtual bool onMouseDown(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onMouseMove(phi::mouseEventArgs* e) override;
    virtual bool onMouseUp(phi::mouseEventArgs* e, phi::inputKey key) override;
    virtual bool onMouseWheel(phi::mouseEventArgs* e, phi::inputKey key) override;

    virtual void update() override;
};

#endif
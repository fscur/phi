#ifndef _PHI_SELECT_AT_POSITION_COMMAND_H_
#define _PHI_SELECT_AT_POSITION_COMMAND_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include "command.h"
#include "globals.h"
#include "renderingCommunicationBuffer.h"
#include "sceneObject.h"
#include "selectObjectCommand.h"

class selectAtPositionCommand :
    public command
{
private:
    phi::bufferRequest* _bufferRequest;
    phi::pickRequest* _pickRequest;
    glm::vec2 _position;
    glm::vec3 _cameraPos;
    glm::vec3 _cameraRight;
    glm::vec3 _cameraUp;
    glm::vec2 _startPos;
    float _eyeZ;
    GLfloat _zBufferValue;
    unsigned int _meshId;
    unsigned int _objectId;
    bool _initialized;
    selectObjectCommand* _selectCommand;

private:
    void init();

protected:
    void execute() override;
    void executeUndo() override;

public:
    selectAtPositionCommand(glm::vec2 position);
    ~selectAtPositionCommand();
};

#endif
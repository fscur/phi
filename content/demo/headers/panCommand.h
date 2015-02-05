#ifndef _PHI_PAN_COMMAND_H_
#define _PHI_PAN_COMMAND_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include "command.h"
#include "globals.h"
#include "renderingCommunicationBuffer.h"

namespace phi
{
    class panCommand :
        public command
    {
    private:
        bufferRequest* _request;
        GLfloat _zBufferValue;
        glm::vec3 _cameraPos;
        glm::vec3 _cameraRight;
        glm::vec3 _cameraUp;
        glm::vec2 _startPos;
        float _eyeZ;
    public:
        panCommand();
        ~panCommand();
        void init() override;
        bool canExecute() override;
        void update() override;
    };
}
#endif
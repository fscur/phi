#ifndef _PHI_ROTATE_COMMAND_H_
#define _PHI_ROTATE_COMMAND_H_

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
    class rotateCommand :
        public command
    {
    private:
        bufferRequest* _request;
        GLfloat _zBufferValue;
        glm::vec3 _targetPos;
        float _zEye;
    public:
        rotateCommand();
        ~rotateCommand();
        virtual void init() override;
        virtual bool canExecute() override;
        void update() override;
    };
}
#endif
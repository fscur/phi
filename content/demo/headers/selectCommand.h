#ifndef _PHI_SELECT_COMMAND_H_
#define _PHI_SELECT_COMMAND_H_

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
    class selectCommand :
        public command
    {
    private:
        bufferRequest* _bufferRequest;
        pickRequest* _pickRequest;
        glm::vec3 _cameraPos;
        glm::vec3 _cameraRight;
        glm::vec3 _cameraUp;
        glm::vec2 _startPos;
        float _eyeZ;
        GLfloat _zBufferValue;
        GLuint _id;
    public:
        selectCommand();
        ~selectCommand();
        void init() override;
        bool canExecute() override;
        void update() override;
    };
}
#endif
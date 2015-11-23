#ifndef _PHI_RENDERING_COMMUNICATION_BUFFER_H_
#define _PHI_RENDERING_COMMUNICATION_BUFFER_H_

#include <vector>
#include <mutex>
#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include "rendering.h"
#include "pickingFrameBuffer.h"

namespace phi
{
    class bufferRequest
    {
    public:
        bufferRequest(unsigned int x, unsigned int y, unsigned int width, unsigned int height, frameBuffer* buffer) :
            x(x), y(y), width(width), height(height), buffer(buffer), completed(false)
        {
        }

        bufferRequest::bufferRequest(const bufferRequest& other) :
            x(other.x), y( other.y), width(other.width), height(other.height), buffer(other.buffer), completed(other.completed), result(other.result)
        {
        }

        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        frameBuffer* buffer;
        mutable std::mutex mutex;
        bool completed;
        GLfloat result;
    };

    class pickRequest
    {
    public:
        pickRequest(unsigned int x, unsigned int y, unsigned int width, unsigned int height, pickingFrameBuffer* buffer) :
            x(x), y(y), width(width), height(height), buffer(buffer), completed(false)
        {
        }

        pickRequest::pickRequest(const pickRequest& other) :
            x(other.x), y( other.y), width(other.width), height(other.height), buffer(other.buffer), completed(other.completed), result(other.result)
        {
        }

        unsigned int x;
        unsigned int y;
        unsigned int width;
        unsigned int height;
        pickingFrameBuffer* buffer;
        mutable std::mutex mutex;
        bool completed;
        GLuint result;
    };

    class renderingCommunicationBuffer
    {
    private:
        static std::vector<bufferRequest*> bufferRequests;
        static std::vector<pickRequest*> pickRequests;
        static std::mutex bufferRequestsMutex;
        static std::mutex pickRequestsMutex;

    public:
        RENDERING_API static void request(bufferRequest* request);
        RENDERING_API static void request(pickRequest* request);
        RENDERING_API static void removeRequest(bufferRequest* request);
        RENDERING_API static void removeRequest(pickRequest* request);
        RENDERING_API static void update();
    };
}

#endif
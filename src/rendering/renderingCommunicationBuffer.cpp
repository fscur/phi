#include <phi/rendering/renderingCommunicationBuffer.h>

namespace phi
{
    std::vector<bufferRequest*> renderingCommunicationBuffer::bufferRequests;
    std::vector<pickRequest*> renderingCommunicationBuffer::pickRequests;
    std::mutex renderingCommunicationBuffer::bufferRequestsMutex;
    std::mutex renderingCommunicationBuffer::pickRequestsMutex;

    void renderingCommunicationBuffer::request(bufferRequest* request)
    {
        bufferRequestsMutex.lock();
        bufferRequests.push_back(request);
        bufferRequestsMutex.unlock();
    }

    void renderingCommunicationBuffer::request(pickRequest* request)
    {
        pickRequestsMutex.lock();
        pickRequests.push_back(request);
        pickRequestsMutex.unlock();
    }

    void renderingCommunicationBuffer::removeRequest(bufferRequest* request)
    {
        bufferRequestsMutex.lock();

        unsigned int i;
        for (i = 0; i < bufferRequests.size(); i++)
        {
            if (bufferRequests[i] == request)
                break;
        }

        bufferRequests.erase(bufferRequests.begin() + i);

        bufferRequestsMutex.unlock();
    }

    void renderingCommunicationBuffer::removeRequest(pickRequest* request)
    {
        pickRequestsMutex.lock();

        unsigned int i;
        for (i = 0; i < pickRequests.size(); i++)
        {
            if (pickRequests[i] == request)
                break;
        }

        pickRequests.erase(pickRequests.begin() + i);

        pickRequestsMutex.unlock();
    }

    void renderingCommunicationBuffer::update()
    {
        if (bufferRequestsMutex.try_lock())
        {
            for (unsigned int i = 0; i < bufferRequests.size(); i++)
            {
                bufferRequest* current = bufferRequests[i];

                if (!current->mutex.try_lock())
                    continue;

                if (current->completed)
                {
                    current->mutex.unlock();
                    continue;
                }

                current->result = current->buffer->getZBufferValue(vec2(current->x, current->y));
                current->completed = true;
                current->mutex.unlock();
            }
            bufferRequestsMutex.unlock();
        }

        if (pickRequestsMutex.try_lock())
        {
            for (unsigned int i = 0; i < pickRequests.size(); i++)
            {
                pickRequest* current = pickRequests[i];

                if (!current->mutex.try_lock())
                    continue;

                if (current->completed)
                {
                    current->mutex.unlock();
                    continue;
                }

                current->result = current->buffer->pick(vec2(current->x, current->y));
                current->completed = true;
                current->mutex.unlock();
            }
            pickRequestsMutex.unlock();
        }
    }
}
#include "renderingCommunicationBuffer.h"


namespace phi
{
    std::vector<bufferRequest*> renderingCommunicationBuffer::bufferRequests;
    std::vector<pickRequest*> renderingCommunicationBuffer::pickRequests;
    bool renderingCommunicationBuffer::bufferLocked = false;
    bool renderingCommunicationBuffer::pickLocked = false;

    void renderingCommunicationBuffer::request(bufferRequest* request)
    {
        while(!bufferLocked);
        bufferLocked = true;
        bufferRequests.push_back(request);
        bufferLocked = false;
    }

    void renderingCommunicationBuffer::request(pickRequest* request)
    {
        while(!pickLocked);
        pickLocked = true;
        pickRequests.push_back(request);
        pickLocked = false;
    }

    void renderingCommunicationBuffer::removeRequest(bufferRequest* request)
    {
        while(!bufferLocked);
        bufferLocked = true;

        unsigned int i;
        for (i = 0; i < bufferRequests.size(); i++)
        {
            if (bufferRequests[i] == request)
                break;
        }

        bufferRequests.erase(bufferRequests.begin() + i);

        bufferLocked = false;
    }

    void renderingCommunicationBuffer::removeRequest(pickRequest* request)
    {
        while(!pickLocked);
        pickLocked = true;

        unsigned int i;

        for (i = 0; i < pickRequests.size(); i++)
        {
            if (pickRequests[i] == request)
                break;
        }

        pickRequests.erase(pickRequests.begin() + i);
        
        pickLocked = false;
    }

    void renderingCommunicationBuffer::update()
    {
        if (!bufferLocked)
        {
            bufferLocked = true;

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

                current->result = current->buffer->getZBufferValue(glm::vec2(current->x, current->y));
                current->completed = true;
                current->mutex.unlock();
            }

            bufferLocked = false;
        }
        
        if (!pickLocked)
        {
            pickLocked = true;

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

                current->result = current->buffer->pick(glm::vec2(current->x, current->y));
                current->completed = true;
                current->mutex.unlock();
            }

            pickLocked = false;
        }
    }
}
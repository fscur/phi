#ifndef _PHI_ZOOM_COMMAND_H_
#define _PHI_ZOOM_COMMAND_H_

#include "command.h"
#include "globals.h"
#include "renderingCommunicationBuffer.h"

namespace phi
{
    class zoomCommand :
        public command
    {
    private:
        bufferRequest* _request;
        bool _in;
        GLfloat _zBufferValue;
    public:
        zoomCommand(bool in);
        ~zoomCommand();
        void init() override;
        bool canExecute() override;
        void finish() override;
    };
}
#endif
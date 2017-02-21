#pragma once
#include <phi.h>

namespace phi
{
    class shaderCompilationResult
    {
    private:
        bool _succeeded;
        GLuint _shaderId;
        string _fileName;
        string _resultMessage;

    public:
        shaderCompilationResult(bool succeeded, GLuint shaderId, string fileName, string resultMessage) :
            _succeeded(succeeded),
            _shaderId(shaderId),
            _fileName(fileName),
            _resultMessage(resultMessage)
        {
        }

        string toString()
        {
            auto message = string();

            message.append(_fileName);
            message.append(" id: " + std::to_string(_shaderId));

            if (_succeeded)
            {
                message.append(" succeeded compilation");
            }
            else
            {
                message.append(" failed to the ground \\m/");
                message.append("\n");
                message.append("Error message: ");
                message.append("\n");
                message.append(_resultMessage);
            }

            message.append("\n");

            return message;
        }

        bool getSucceeded() { return _succeeded; }
    };
}
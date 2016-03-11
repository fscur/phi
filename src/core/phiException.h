#pragma once
#include <phi.h>

namespace phi
{
    class phiException : public exception
    {
    private:
        char* _message;
    public:
        phiException(char* message);
        ~phiException();
        virtual const char* what() const noexcept override;
    };
}
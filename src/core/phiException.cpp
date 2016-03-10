#include <precompiled.h>
#include "phiException.h"

namespace phi
{
    phiException::phiException(char* message) :
        exception::exception(),
        _message(message)
    {
        logError(_message);
    }

    phiException::~phiException()
    {
    }

    const char* phiException::what() const
    {
        return _message;
    }
}
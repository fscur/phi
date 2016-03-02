#include <precompiled.h>
#include "console.h"

namespace phi
{
    void console::write(string message)
    {
        std::cout << message;
    }

    void console::writeLine(string message)
    {
        std::cout << message << std::endl;
    }
}
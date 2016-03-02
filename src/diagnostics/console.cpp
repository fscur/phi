#include "diagnostics.h"
#include "console.h"
#include <iostream>

namespace phi
{
    void console::write(std::string message)
    {
        std::cout << message;
    }

    void console::writeLine(std::string message)
    {
        std::cout << message << std::endl;
    }
}
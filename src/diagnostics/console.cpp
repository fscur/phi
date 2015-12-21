#include <phi\diagnostics\console.h>

void console::write(std::string message)
{
    std::cout << message;
}

void console::writeLine(std::string message)
{
    std::cout << message << std::endl;
}
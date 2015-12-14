#include <phi\diagnostics\console.h>

void console::Write(std::string message)
{
    std::cout << message;
}

void console::WriteLine(std::string message)
{
    std::cout << message << std::endl;
}
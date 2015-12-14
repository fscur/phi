#ifndef _PHI_CONSOLE_H_
#define _PHI_CONSOLE_H_

#include <string>
#include <iostream>

class console
{
public:
    console() { };
    ~console() { };
    static void Write(std::string message);
    static void WriteLine(std::string message);
};

#endif
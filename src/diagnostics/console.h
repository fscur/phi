#ifndef _PHI_CONSOLE_H_
#define _PHI_CONSOLE_H_

#include <string>
#include <iostream>

class console
{
public:
    console() { };
    ~console() { };
    static void write(std::string message);
    static void writeLine(std::string message);
};

#endif
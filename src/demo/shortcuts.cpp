#include <precompiled.h>
#include "shortcuts.h"

bool shortcuts::add(phi::string name, phi::inputKey key)
{
    if (_commands.find(key) == _commands.end())
    {
        _commands[key] = name;
        return true;
    }
    else
        return false;
}

phi::string shortcuts::getCommand(phi::inputKey key)
{
    if (_commands.find(key) == _commands.end())
        return "";

    return _commands[key];
}

phi::string shortcuts::operator[](phi::inputKey key)
{
    return getCommand(key);
}
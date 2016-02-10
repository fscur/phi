#include "shortcuts.h"

bool shortcuts::add(std::string name, phi::inputKey key)
{
    if (_commands.find(key) == _commands.end())
    {
        _commands[key] = name;
        return true;
    }
    else
        return false;
}

std::string shortcuts::getCommand(phi::inputKey key)
{
    if (_commands.find(key) == _commands.end())
        return "";

    return _commands[key];
}

std::string shortcuts::operator[](phi::inputKey key)
{
    return getCommand(key);
}
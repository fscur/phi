#pragma once
#include <core\command.h>

namespace phi
{
    struct shortcut
    {
        shortcut(std::vector<keycode> keys, std::function<command*(void)> commandFunc) :
            keys(keys),
            commandFunc(commandFunc)
        {
        }

        std::vector<keycode> keys;
        std::function<command*(void)> commandFunc;
    };
}
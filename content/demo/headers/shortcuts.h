#ifndef _SHORTCUTS_H_
#define _SHORTCUTS_H_

#include <unordered_map>
#include <string>
#include "inputKey.h"

static const unsigned int PHI_TOTAL_KEYS = 310;

static const unsigned int PHI_NONE = 0;
static const unsigned int PHI_DOWN = 1;
static const unsigned int PHI_PRESSED = 2;
static const unsigned int PHI_UP = 4;
static const unsigned int PHI_CTRL_PRESSED = 1;
static const unsigned int PHI_SHIFT_PRESSED = 2;
static const unsigned int PHI_ALT_PRESSED = 4;

static const unsigned long PHI_MOUSE_LEFT = 301;
static const unsigned long PHI_MOUSE_MIDDLE = 302;
static const unsigned long PHI_MOUSE_RIGHT = 303;
static const unsigned long PHI_MOUSE_WHEEL_UP = 304;
static const unsigned long PHI_MOUSE_WHEEL_DOWN = 305;

class shortcuts
{
private:
    std::unordered_map<phi::inputKey, std::string, phi::inputKeyHasher> _commands;

public:
    bool add(std::string name, phi::inputKey key);
    std::string getCommand(phi::inputKey key);
    std::string operator[](phi::inputKey key);
    std::unordered_map<phi::inputKey, std::string, phi::inputKeyHasher> getCommands() { return _commands; }
};

#endif
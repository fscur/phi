#ifndef _PHI_COMMANDS_MANAGER_H_
#define _PHI_COMMANDS_MANAGER_H_

#include <glm\glm.hpp>
#include <unordered_map>
#include <vector>
#include <functional>
#include "command.h"
#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"

namespace phi
{
    class inputKey
    {
    public:
        int key;
        unsigned int modifiers;

    public:
        inputKey(int key, unsigned int modifiers) :
            key(key), modifiers(modifiers)
        {
        }

        bool operator==(const inputKey &other) const
        {
            return
                key == other.key &&
                modifiers == other.modifiers;
        }
    };

    struct inputKeyHasher
    {
        std::size_t operator()(const phi::inputKey& k) const
        {
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((std::hash<int>()(k.key) ^
                (std::hash<unsigned int>()(k.modifiers) << 1)) >> 1);
        }
    };

    class commandsManager
    {
    private:
        static const unsigned int TOTAL_KEYS = 310;

        static const unsigned int NONE = 0;
        static const unsigned int DOWN = 1;
        static const unsigned int PRESSED = 2;
        static const unsigned int UP = 4;
        static const unsigned int CTRL_PRESSED = 1;
        static const unsigned int SHIFT_PRESSED = 2;
        static const unsigned int ALT_PRESSED = 4;

        static const unsigned long PHI_MOUSE_LEFT = 301;
        static const unsigned long PHI_MOUSE_MIDDLE = 302;
        static const unsigned long PHI_MOUSE_RIGHT = 303;
        static const unsigned long PHI_MOUSE_WHEEL_UP = 304;
        static const unsigned long PHI_MOUSE_WHEEL_DOWN = 305;

        bool _isMouseDown;
        bool _isCtrlPressed;
        bool _isShiftPressed;
        bool _isAltPressed;

        mouseEventArgs _mouseDownEventArgs;
        mouseEventArgs _keyboardDownEventArgs;

        commandInfo* _commandInfo;

        unsigned int _modifiers;
        unsigned int _states[TOTAL_KEYS];
        std::unordered_map<inputKey, std::function<command*(void)>, inputKeyHasher> _commands;
        std::unordered_map<int, command*> _pendingCommands;
        std::unordered_map<int, command*> _executingCommands;
    public:
        commandsManager();
        ~commandsManager();

        void onBeginInput(phi::size<unsigned int> viewportSize);
        bool onMouseDown(mouseEventArgs* e);
        bool onMouseMove(mouseEventArgs* e);
        bool onMouseUp(mouseEventArgs* e);
        bool onMouseWheel(mouseEventArgs* e);
        bool onKeyDown(keyboardEventArgs e);
        bool onKeyUp(keyboardEventArgs e);
        void onEndInput();
        void update();
    };
}

#endif
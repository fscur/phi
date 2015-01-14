#ifndef _PHI_COMMANDS_MANAGER_H_
#define _PHI_COMMANDS_MANAGER_H_

#include <glm\glm.hpp>
#include <map>
#include "command.h"
#include "mouseEventArgs.h"
#include "keyboardEventArgs.h"

namespace phi
{
	class commandsManager
	{
	private:

		static const unsigned long L_MOUSE_DOWN = 1;
		static const unsigned long M_MOUSE_DOWN = 2;
		static const unsigned long R_MOUSE_DOWN = 4;
		static const unsigned long L_MOUSE_PRESSED = 8;
		static const unsigned long M_MOUSE_PRESSED = 16;
		static const unsigned long R_MOUSE_PRESSED = 32;
		static const unsigned long MOUSE_MOVE = 64;
		static const unsigned long L_MOUSE_UP = 128;
		static const unsigned long M_MOUSE_UP = 256;
		static const unsigned long R_MOUSE_UP = 512;
		static const unsigned long MOUSE_WHEEL = 1024;
		static const unsigned long CTRL_PRESSED = 2048;
		static const unsigned long SHIFT_PRESSED = 4096;
		static const unsigned long ALT_PRESSED = 8192;

		bool _isMouseDown;
		bool _isCtrlPressed;
		bool _isShiftPressed;
		bool _isAltPressed;

		mouseEventArgs _mouseDownEventArgs;
		mouseEventArgs _keyboardDownEventArgs;

		unsigned long _actionsSum;
		commandInfo* _commandInfo;
		std::map<unsigned long, command*> _commands; 
	public:
		commandsManager();
		~commandsManager();

		void onBeginInput(phi::size<unsigned int> viewportSize);
		bool onMouseDown(mouseEventArgs e);
		bool onMouseMove(mouseEventArgs e);
		bool onMouseUp(mouseEventArgs e);
		bool onMouseWheel(mouseEventArgs e);
		bool onKeyDown(keyboardEventArgs e);
		bool onKeyUp(keyboardEventArgs e);
		void onEndInput();
	};
}
#endif
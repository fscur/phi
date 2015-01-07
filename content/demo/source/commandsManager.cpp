#include "commandsManager.h"

#include "panCommand.h"
#include "rotateCommand.h"
#include "zoomCommand.h"

namespace phi
{
	commandsManager::commandsManager()
	{
		_isMouseDown = false;
		_isCtrlPressed = false;
		_isShiftPressed = false;
		_isAltPressed = false;

		_actionsSum = 0;
		_commandInfo = new commandInfo();
		_commands[M_MOUSE_DOWN] = new panStartCommand();
		_commands[M_MOUSE_PRESSED + MOUSE_MOVE] = new panCommand();
		_commands[R_MOUSE_DOWN] = new rotateStartCommand();
		_commands[R_MOUSE_PRESSED + MOUSE_MOVE] = new rotateCommand();
		_commands[MOUSE_WHEEL] = new zoomCommand();
	}

	commandsManager::~commandsManager()
	{
		delete(_commandInfo);

		std::map<unsigned long, command*>::iterator i;

		for(i = _commands.begin(); i != _commands.end(); i++)
		{
			DELETE(i->second);
		}
	}

	void commandsManager::onBeginInput(phi::size<unsigned int> viewportSize)
	{
		if (_commandInfo == nullptr)
			_commandInfo = new commandInfo();

		_actionsSum = 0;

		if (_isMouseDown)
		{
			if (_mouseDownEventArgs.leftButtonPressed)
				_actionsSum |= L_MOUSE_PRESSED;
			
			if (_mouseDownEventArgs.middleButtonPressed)
				_actionsSum |= M_MOUSE_PRESSED;

			if (_mouseDownEventArgs.rightButtonPressed)
				_actionsSum |= R_MOUSE_PRESSED;
		}

		if (_isCtrlPressed)
			_actionsSum |= CTRL_PRESSED;
		
		if (_isShiftPressed)
			_actionsSum |= SHIFT_PRESSED;
		
		if (_isAltPressed)
			_actionsSum |= ALT_PRESSED;

		_commandInfo->viewportSize = viewportSize;
	}

	bool commandsManager::onMouseDown(mouseEventArgs e)
	{
		if (_isMouseDown)
			return false;

		if (e.leftButtonPressed)
			_actionsSum |= L_MOUSE_DOWN;
			
		if (e.middleButtonPressed)
			_actionsSum |= M_MOUSE_DOWN;

		if (e.rightButtonPressed)
			_actionsSum |= R_MOUSE_DOWN;

		_isMouseDown = true;
		_mouseDownEventArgs = e;
		_commandInfo->mouseDownPos = glm::vec2(e.x, e.y);
		_commandInfo->mousePos = glm::vec2(e.x, e.y);
		
		return true;
	}

	bool commandsManager::onMouseMove(mouseEventArgs e)
	{
		_actionsSum |= MOUSE_MOVE;

		_commandInfo->lastMousePos = _commandInfo->mousePos;
		_commandInfo->mousePos = glm::vec2(e.x, e.y);
		return true;
	}

	bool commandsManager::onMouseUp(mouseEventArgs e)
	{
		if (!_isMouseDown)
			return false;

		_actionsSum |= MOUSE_UP;

		_commandInfo->mousePos = glm::vec2(e.x, e.y);

		_isMouseDown = false;

		_mouseDownEventArgs.leftButtonPressed = false;
		_mouseDownEventArgs.middleButtonPressed = false;
		_mouseDownEventArgs.rightButtonPressed = false;

		return true;
	}

	bool commandsManager::onMouseWheel(mouseEventArgs e)
	{
		_actionsSum |= MOUSE_WHEEL;

		_commandInfo->wheelDelta = e.wheelDelta;
		_commandInfo->mousePos = glm::vec2(e.x, e.y);

		return true;
	}

	bool commandsManager::onKeyDown(keyboardEventArgs e)
	{
		_isCtrlPressed = e.isCtrlPressed;
		_isShiftPressed = e.isShiftPressed;
		_isAltPressed = e.isAltPressed;

		if (_isCtrlPressed)
			_actionsSum |= CTRL_PRESSED;

		if (_isShiftPressed)
			_actionsSum |= SHIFT_PRESSED;

		if(_isAltPressed)
			_actionsSum |= ALT_PRESSED;

		return true;
	}

	bool commandsManager::onKeyUp(keyboardEventArgs e)
	{
		_isCtrlPressed = false;
		_isShiftPressed = false;
		_isAltPressed = false;

		return true;
	}

	void commandsManager::onEndInput()
	{
		command* command = _commands[_actionsSum];

		if (command != nullptr)
		{
			_commandInfo = command->execute(_commandInfo);
		}
	}
}
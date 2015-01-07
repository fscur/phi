#ifndef _PHI_ROTATE_COMMAND_H_
#define _PHI_ROTATE_COMMAND_H_

#include "command.h"
#include "globals.h"

namespace phi
{
	class rotateStartCommandInfo : 
		public commandInfo
	{
	public:
		rotateStartCommandInfo(commandInfo info)
		{
			this->mouseDownPos = info.mouseDownPos;
			this->mousePos = info.mousePos;
			this->viewportSize = info.viewportSize;
		}

		~rotateStartCommandInfo(){}
		glm::vec3 targetPos;
		float zEye;
	};

	class rotateStartCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};

	class rotateCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};
}
#endif
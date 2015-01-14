#ifndef _PHI_ROTATE_COMMAND_H_
#define _PHI_ROTATE_COMMAND_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

#include "command.h"
#include "globals.h"

namespace phi
{
	class rotateCommandInfo : 
		public commandInfo
	{
	public:
		rotateCommandInfo(commandInfo info)
		{
			this->mouseDownPos = info.mouseDownPos;
			this->mousePos = info.mousePos;
			this->viewportSize = info.viewportSize;
		}

		~rotateCommandInfo(){}
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

	class rotateEndCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};
}
#endif
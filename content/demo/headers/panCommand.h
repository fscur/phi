#ifndef _PHI_PAN_COMMAND_H_
#define _PHI_PAN_COMMAND_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

#include "command.h"
#include "globals.h"

namespace phi
{
	class panCommandInfo : 
		public commandInfo
	{
	public:
		panCommandInfo(commandInfo info)
		{
			this->mousePos = info.mousePos;
			this->viewportSize = info.viewportSize;
		}

		~panCommandInfo(){}
		glm::vec3 cameraPos;
		glm::vec3 cameraRight;
		glm::vec3 cameraUp;
		glm::vec2 startPos;
		float eyeZ;
	};

	class panStartCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};

	class panCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};

	class panEndCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};
}
#endif
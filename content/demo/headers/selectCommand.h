#ifndef _PHI_SELECT_COMMAND_H_
#define _PHI_SELECT_COMMAND_H_

#if WIN32
    #include <GL/glew.h>
#else
    #include <OpenGL/gl3.h>
#endif

#include "command.h"
#include "globals.h"

namespace phi
{
	class selectCommandInfo : 
		public commandInfo
	{
	public:
		selectCommandInfo(commandInfo info)
		{
			this->mousePos = info.mousePos;
			this->viewportSize = info.viewportSize;
		}

		~selectCommandInfo(){}
		glm::vec3 cameraPos;
		glm::vec3 cameraRight;
		glm::vec3 cameraUp;
		glm::vec2 startPos;
		float eyeZ;
	};

	class selectCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};
}
#endif
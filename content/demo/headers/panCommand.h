#ifndef _PHI_PAN_COMMAND_H_
#define _PHI_PAN_COMMAND_H_

#include "command.h"
#include "globals.h"

namespace phi
{
	class panStartCommandInfo : 
		public commandInfo
	{
	public:
		panStartCommandInfo(commandInfo info)
		{
			this->mousePos = info.mousePos;
			this->viewportSize = info.viewportSize;
		}

		~panStartCommandInfo(){}
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
}
#endif
#ifndef _PHI_COMMAND_H_
#define _PHI_COMMAND_H_

#include <glm\glm.hpp>
#include "size.h"

namespace phi
{
	class commandInfo
	{
	public:
		glm::vec2 mouseDownPos;
		glm::vec2 mousePos;
		glm::vec2 lastMousePos;
		float wheelDelta;
		phi::size<unsigned int> viewportSize;
	public:
		commandInfo(){}
		virtual ~commandInfo(){}
	};

	class command
	{
	public:
		virtual commandInfo* execute(commandInfo* info) = 0;
	};
}
#endif
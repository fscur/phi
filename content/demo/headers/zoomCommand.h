#ifndef _PHI_ZOOM_COMMAND_H_
#define _PHI_ZOOM_COMMAND_H_

#include "command.h"
#include "globals.h"

namespace phi
{
	class zoomCommand :
		public command
	{
	public:
		commandInfo* execute(commandInfo* info) override;
	};
}
#endif
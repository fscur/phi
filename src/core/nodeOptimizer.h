#pragma once
#include <phi.h>
#include "coreApi.h"
#include "node.h"

namespace phi
{
	class nodeOptimizer
	{
	private:
		static void removeEmptyNodes(node* node);
		static void removeUselessNodes(node* node);

	public:
		CORE_API static node* optimize(node*);
	};
}

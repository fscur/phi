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
		static node* removeUselessNodes(node* node);
		static void deleteNodes(vector<node*>& nodes);
		static bool isNodeUseless(phi::node* node);
		static bool isNodeEmpty(phi::node* node);

	public:
		CORE_API static node* optimize(node*);
	};
}

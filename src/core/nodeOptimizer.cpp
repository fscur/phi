#include <precompiled.h>
#include "nodeOptimizer.h"

namespace phi
{
	void nodeOptimizer::removeEmptyNodes(node* node)
	{
		/*vector<phi::node*> toRemove;
		auto children = node->getChildren();

		for (auto child : *children)
		{
			if (child->getChildren()->size() == 0 && child->getComponents()->size() == 0)
				toRemove.push_back(child);

			removeEmptyNodes(child);
		}

		for (auto nodeToRemove : toRemove)
		{
			auto it = std::find(children->begin(), children->end(), nodeToRemove);
			children->erase(it);
			safeDelete(nodeToRemove);
		}*/
	}

	void nodeOptimizer::removeUselessNodes(node* node)
	{
		//auto children = node->getChildren();
		//auto childrenCount = children->size();

		//for (size_t i = 0; i < childrenCount; ++i)
		//{
		//	auto child = children->at(i);

		//	bool doesNotHaveComponents = child->getComponents()->size() == 0;
		//	bool hasOnlyOneChild = child->getChildren()->size() == 1;
		//	vector<phi::node*> toRemove;

		//	if (doesNotHaveComponents && hasOnlyOneChild)
		//	{
		//		auto next = child;
		//		while (next->getChildren()->size() == 1 && next->getComponents()->size() == 0)
		//		{
		//			toRemove.push_back(next);
		//			next = next->getChildren()->at(0);
		//		}

		//		//next->_parent = this;
		//		next->setParent(node);
		//		(*children)[i] = child = next;
		//	}

		//	for (auto nodeToRemove : toRemove)
		//		nodeToRemove->getChildren()->clear();

		//	for (auto nodeToRemove : toRemove)
		//		safeDelete(nodeToRemove);

		//	removeUselessNodes(child);
		//}
	}

	node* nodeOptimizer::optimize(node* node)
	{
		/*removeEmptyNodes(node);
		removeUselessNodes(node);

		auto children = node->getChildren();
		bool doesNotHaveComponents = node->getComponents()->size() == 0;
		bool hasOnlyOneChild = children->size() == 1;

		if (doesNotHaveComponents && hasOnlyOneChild)
		{
			auto child = children->at(0);

			auto grandsons = children->at(0)->getChildren();
			children->clear();

			auto grandsonsCount = grandsons->size();
			for (size_t i = 0; i < grandsonsCount; ++i)
			{
				auto grandson = grandsons->at(i);
				grandson->setParent(node);
				children->push_back(grandson);
			}

			child->getChildren()->clear();
			safeDelete(child);
		}

		return node;*/

		return nullptr;
	}
}
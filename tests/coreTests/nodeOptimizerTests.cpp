#include <precompiled.h>
#include <gtest\gtest.h>
#include <core\node.h>
#include <core\nodeOptimizer.h>
#include <core\mesh.h>

using namespace phi;

TEST(nodeOptimizer, optimize_rootNodeWithOneChild_childBecomesRoot)
{
	//Arrange
	auto root = new node("root");
	auto child = new node("child");
	auto component = new mesh("mesh", nullptr, nullptr);
	child->addComponent(component);
	root->addChild(child);

	//Act
	auto result = nodeOptimizer::optimize(root);

	//Assert
	auto childCount = result->getChildren()->size();
	auto expectedChildCount = 0;

	auto componentCount = result->getComponents()->size();
	auto expectedComponentCount = 1;

	auto componentName = (*result->getComponents())[0]->getName();
	auto expectedComponentName = string("mesh");

	ASSERT_EQ(expectedChildCount, childCount);
	ASSERT_EQ(expectedComponentCount, componentCount);
	ASSERT_EQ(expectedComponentName, componentName);
}
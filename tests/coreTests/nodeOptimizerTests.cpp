#include <precompiled.h>
#include <phi.h>
#include <gtest\gtest.h>
#include <core\node.h>
#include <core\nodeOptimizer.h>
#include <core\mesh.h>

using namespace phi;

TEST(nodeOptimizer, optimize_nodeTreeWithOneUselessNode_usefullChildBecomesRootAndPreservesTransform)
{
    //Arrange
    auto root = new node("root");
    root->getTransform()->translate(vec3(10.0f, 0.0f, 0.0f));
    auto child = new node("child");
    auto component = new mesh(nullptr, nullptr);
    child->addComponent(component);
    root->addChild(child);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedChildCount = 0u;
    auto childCount = result->getChildren()->size();

    auto expectedComponentCount = 1u;
    auto componentCount = result->getComponents()->size();

    auto expectedLocalPosition = vec3(10.0f, 0.0f, 0.0f);
    auto localPosition = result->getTransform()->getPosition();

    auto expectedParent = nullptr;
    auto parent = result->getParent();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChildCount, childCount);
    ASSERT_EQ(expectedComponentCount, componentCount);
    ASSERT_EQ(expectedLocalPosition, localPosition);
}

TEST(nodeOptimizer, optimize_nodeTreeWithTwoUselessNodes_usefullChildBecomesRootAndPreservesTransform)
{
    //Arrange
    auto root = new node("root");
    auto child = new node("child");
    auto grandson = new node("grandson");

    root->getTransform()->translate(vec3(10.0f, 0.0f, 0.0f));
    root->addChild(child);

    child->getTransform()->rotate(phi::PI_OVER_2, vec3(0.0f, 1.0f, 0.0f));
    child->addChild(grandson);

    auto component = new mesh(nullptr, nullptr);
    grandson->addComponent(component);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedChildCount = 0u;
    auto childCount = result->getChildren()->size();

    auto expectedComponentCount = 1u;
    auto componentCount = result->getComponents()->size();

    auto expectedNodeName = string("grandson");
    auto nodeName = result->getName();

    auto expectedLocalPosition = vec3(10.0f, 0.0f, 0.0f);
    auto localPosition = result->getTransform()->getLocalPosition();

    auto expectedLocalOrientation = quat(vec3(0.0f, phi::PI_OVER_2, 0.0f));
    auto localOrientation = result->getTransform()->getLocalOrientation();

    auto expectedParent = nullptr;
    auto parent = result->getParent();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChildCount, childCount);
    ASSERT_EQ(expectedComponentCount, componentCount);
    ASSERT_EQ(expectedNodeName, nodeName);
    ASSERT_EQ(expectedLocalPosition, localPosition);
    ASSERT_EQ(expectedLocalOrientation, localOrientation);
}

TEST(nodeOptimizer, optimize_nodeTreeWithARootAndTwoChildreNodesWithComponents_treeStaysTheSame)
{
    //Arrange
    auto root = new node("root");
    auto child0 = new node("child0");
    auto child1 = new node("child1");

    root->getTransform()->translate(vec3(10.0f, 0.0f, 0.0f));
    root->addChild(child0);
    root->addChild(child1);

    auto component0 = new mesh(nullptr, nullptr);
    child0->addComponent(component0);

    auto component1 = new mesh(nullptr, nullptr);
    child1->addComponent(component1);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedChildCount = 2u;
    auto childCount = result->getChildren()->size();

    auto expectedComponentCount = 0u;
    auto componentCount = result->getComponents()->size();

    auto expectedNodeName = string("root");
    auto nodeName = result->getName();

    auto expectedLocalPosition = vec3(10.0f, 0.0f, 0.0f);
    auto localPosition = result->getTransform()->getLocalPosition();

    auto expectedParent = nullptr;
    auto parent = result->getParent();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChildCount, childCount);
    ASSERT_EQ(expectedComponentCount, componentCount);
    ASSERT_EQ(expectedNodeName, nodeName);
    ASSERT_EQ(expectedLocalPosition, localPosition);
}

TEST(nodeOptimizer, optimize_nodeTreeWithARootAUselessChildNodeAndTwoGrandsonsNodesWithComponents_uselessChildIsRemovedGrandsonsBecomeChildrenAndTransformsArePreserved)
{
    //Arrange
    auto root = new node("root");
    auto child = new node("child");
    auto grandson0 = new node("grandson0");
    auto grandson1 = new node("grandson1");

    root->getTransform()->translate(vec3(10.0f, 0.0f, 0.0f));
    root->addChild(child);

    child->getTransform()->translate(vec3(0.0f, 10.0f, 0.0f));
    child->getTransform()->rotate(phi::PI_OVER_2, vec3(0.0f, 1.0f, 0.0f));

    child->addChild(grandson0);
    child->addChild(grandson1);

    auto component0 = new mesh(nullptr, nullptr);
    grandson0->addComponent(component0);

    auto component1 = new mesh(nullptr, nullptr);
    grandson1->addComponent(component1);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedChildCount = 2u;
    auto childCount = result->getChildren()->size();

    auto expectedComponentCount = 0u;
    auto componentCount = result->getComponents()->size();

    auto expectedNodeName = string("child");
    auto nodeName = result->getName();

    auto expectedLocalPosition = vec3(10.0f, 10.0f, 0.0f);
    auto localPosition = result->getTransform()->getLocalPosition();

    auto expectedLocalOrientation = quat(vec3(0.0f, phi::PI_OVER_2, 0.0f));
    auto localOrientation = result->getTransform()->getLocalOrientation();

    auto expectedParent = nullptr;
    auto parent = result->getParent();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChildCount, childCount);
    ASSERT_EQ(expectedComponentCount, componentCount);
    ASSERT_EQ(expectedNodeName, nodeName);
    ASSERT_EQ(expectedLocalPosition, localPosition);
    ASSERT_EQ(expectedLocalOrientation, localOrientation);
}

TEST(nodeOptimizer, optimize_rootWithTwoChildrenAndOneIsUselessWithSubTree_uselessChildIsRemovedSubTreeGrandsonsBecomeChildrenAndTransformsArePreserved)
{
    //Arrange
    auto root = new node("root");
    auto child0 = new node("child0");
    auto child1 = new node("child1");
    auto grandson0 = new node("grandson0");
    auto grangrandson0 = new node("grangrandson0");
    auto grangrandson1 = new node("grangrandson1");
    auto component0 = new mesh(nullptr, nullptr);
    auto component1 = new mesh(nullptr, nullptr);
    auto component2 = new mesh(nullptr, nullptr);

    root->getTransform()->translate(vec3(10.0f, 0.0f, 0.0f));

    root->addChild(child0);
    root->addChild(child1);

    child0->addComponent(component0);

    child1->addChild(grandson0);
    child1->getTransform()->translate(vec3(0.0f, 10.0f, 0.0f));
    child1->getTransform()->rotate(phi::PI_OVER_2, vec3(0.0f, 1.0f, 0.0f));

    grandson0->addChild(grangrandson0);
    grandson0->addChild(grangrandson1);
    grangrandson0->addComponent(component1);
    grangrandson1->addComponent(component2);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedParent = nullptr;
    auto parent = result->getParent();

    auto expectedChild1Parent = root;
    auto child1Parent = result->getChildren()->at(1)->getParent();

    auto expectedRootChildrenCount = 2u;
    auto rootChildrenCount = result->getChildren()->size();

    auto expectedChild0ChildrenCount = 0u;
    auto child0ChildrenCount = result->getChildren()->at(0)->getChildren()->size();

    auto expectedChild1ChildrenCount = 2u;
    auto child1ChildrenCount = result->getChildren()->at(1)->getChildren()->size();

    auto expectedChild0ComponentCount = 1u;
    auto child0ComponentCount = result->getChildren()->at(0)->getComponents()->size();

    auto expectedChild1ComponentCount = 0u;
    auto child1ComponentCount = result->getChildren()->at(1)->getComponents()->size();

    auto expectedGrandson0ComponentCount = 1u;
    auto grandson0ComponentCount = result->getChildren()->at(1)->getChildren()->at(0)->getComponents()->size();

    auto expectedGrandson1ComponentCount = 1u;
    auto grandson1ComponentCount = result->getChildren()->at(1)->getChildren()->at(1)->getComponents()->size();

    auto expectedNodeName = string("root");
    auto nodeName = result->getName();

    auto expectedChild1NodeName = string("grandson0");
    auto child1NodeName = result->getChildren()->at(1)->getName();

    auto expectedLocalPosition = vec3(10.0f, 0.0f, 0.0f);
    auto localPosition = result->getTransform()->getLocalPosition();

    auto expectedChild1LocalPosition = vec3(0.0f, 10.0f, 0.0f);
    auto child1LocalPosition = result->getChildren()->at(1)->getTransform()->getLocalPosition();

    auto expectedChild1LocalOrientation = quat(vec3(0.0f, phi::PI_OVER_2, 0.0f));
    auto child1LocalOrientation = result->getChildren()->at(1)->getTransform()->getLocalOrientation();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChild1Parent, child1Parent);
    ASSERT_EQ(expectedRootChildrenCount, rootChildrenCount);
    ASSERT_EQ(expectedChild0ChildrenCount, child0ChildrenCount);
    ASSERT_EQ(expectedChild1ChildrenCount, child1ChildrenCount);
    ASSERT_EQ(expectedChild0ComponentCount, child0ComponentCount);
    ASSERT_EQ(expectedChild1ComponentCount, child1ComponentCount);
    ASSERT_EQ(expectedGrandson0ComponentCount, grandson0ComponentCount);
    ASSERT_EQ(expectedGrandson1ComponentCount, grandson1ComponentCount);
    ASSERT_EQ(expectedNodeName, nodeName);
    ASSERT_EQ(expectedChild1NodeName, child1NodeName);
    ASSERT_EQ(expectedLocalPosition, localPosition);
    ASSERT_EQ(expectedChild1LocalPosition, child1LocalPosition);
    ASSERT_EQ(expectedChild1LocalOrientation, child1LocalOrientation);
}

TEST(nodeOptimizer, optimize_rootWithTwoChildrenAndOneEmptyChildNode_returnsRootWithOneComponent)
{
    //Arrange
    auto root = new node("root");
    auto child0 = new node("child0");
    auto child1 = new node("child1");
    auto component0 = new mesh(nullptr, nullptr);

    root->addChild(child0);
    root->addChild(child1);
    child1->addComponent(component0);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedChildCount = 0u;
    auto childCount = result->getChildren()->size();

    auto expectedComponentCount = 1u;
    auto componentCount = result->getComponents()->size();

    auto expectedParent = nullptr;
    auto parent = result->getParent();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChildCount, childCount);
    ASSERT_EQ(expectedComponentCount, componentCount);
}

TEST(nodeOptimizer, optimize_rootWithThreeChildrenOneEmptyOneSubTreeWithOneEmptyNode_returnsRootWithTwoChildrenWithComponents)
{
    //Arrange
    auto root = new node("root");
    auto child0 = new node("child0");
    auto child1 = new node("child1");
    auto child2 = new node("child2");

    auto grandson0 = new node("grandson0");
    auto grandson1 = new node("grandson1");

    auto component0 = new mesh(nullptr, nullptr);
    auto component1 = new mesh(nullptr, nullptr);

    root->addChild(child0);
    root->addChild(child1);
    root->addChild(child2);

    child1->addChild(grandson0);
    child1->addChild(grandson1);

    child2->addComponent(component0);
    grandson1->addComponent(component1);

    //Act
    auto result = nodeOptimizer::optimize(root);

    //Assert
    auto expectedChildCount = 2u;
    auto childCount = result->getChildren()->size();

    auto expectedChild0ComponentCount = 1u;
    auto child0ComponentCount = result->getChildren()->at(0)->getComponents()->size();

    auto expectedChild1ComponentCount = 1u;
    auto child1ComponentCount = result->getChildren()->at(1)->getComponents()->size();

    auto expectedParent = nullptr;
    auto parent = result->getParent();

    ASSERT_EQ(expectedParent, parent);
    ASSERT_EQ(expectedChildCount, childCount);
    ASSERT_EQ(expectedChild0ComponentCount, child0ComponentCount);
    ASSERT_EQ(expectedChild1ComponentCount, child1ComponentCount);    
}
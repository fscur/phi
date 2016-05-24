#include <precompiled.h>
#include <gtest\gtest.h>
#include <fakeit\fakeit.hpp>

#include <core\node.h>
#include <core\component.h>

using namespace fakeit;
using namespace phi;

TEST(node, equals_twoEmptyNodes_shouldBeEqual)
{
    //Arrange
    auto node1 = node();
    auto node2 = node();

    //Act
    auto result = node1 == node2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(node, equals_twoEmptyNodesWithDifferentNames_shouldBeDifferent)
{
    //Arrange
    auto node1 = node("node1");
    auto node2 = node("node2");

    //Act
    auto result = node1 != node2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(node, equals_twoEmptyNodesWithSameName_shouldBeEqual)
{
    //Arrange
    auto node1 = node("node");
    auto node2 = node("node");

    //Act
    auto result = node1 == node2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(node, equals_twoNodesWithDifferentTransform_shouldBeDifferent)
{
    //Arrange
    auto node1 = node("node");
    node1.getTransform()->setLocalPosition(vec3(10.f, 2.f, 0.f));
    auto node2 = node("node");

    //Act
    auto result = node1 != node2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(node, equals_twoNodesWithSameTransform_shouldBeEqual)
{
    //Arrange
    auto node1 = node("node");
    node1.setPosition(vec3(10.f, 2.f, 0.f));
    auto node2 = node("node");
    node2.setPosition(vec3(10.f, 2.f, 0.f));

    //Act
    auto result = node1 == node2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(node, equals_nodeWithSonAndNodeWithoutSon_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("node");
    auto son1 = new node("son1");
    node1->addChild(son1);

    auto node2 = new node("node");

    //Act
    auto result = *node1 != *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoNodesWithSameSon_shouldBeEqual)
{
    //Arrange
    auto node1 = new node("node");
    auto son1 = new node("son");
    node1->addChild(son1);

    auto node2 = new node("node");
    auto son2 = new node("son");
    node2->addChild(son2);

    //Act
    auto result = *node1 == *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoNodesWithSameDisplacedPositionSon_shouldBeEqual)
{
    //Arrange
    auto node1 = new node("node");
    auto son1 = new node("son");
    son1->setPosition(vec3(2.f, 15.f, 0.f));
    node1->addChild(son1);

    auto node2 = new node("node");
    auto son2 = new node("son");
    son2->setPosition(vec3(2.f, 15.f, 0.f));
    node2->addChild(son2);

    //Act
    auto result = *node1 == *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoNodesWithSameSonAndDifferentDisplacedPosition_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("node");
    auto son1 = new node("son");
    son1->setPosition(vec3(2.f, 15.f, 0.f));
    node1->addChild(son1);

    auto node2 = new node("node");
    auto son2 = new node("son");
    son2->setPosition(vec3(20.f, 15.f, 0.f));
    node2->addChild(son2);

    //Act
    auto result = *node1 != *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoNodesWithDifferentSon_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("node");
    auto son1 = new node("son");
    son1->setPosition(vec3(2.f, 15.f, 0.f));
    node1->addChild(son1);

    auto node2 = new node("node");
    auto son2 = new node("son");
    son2->setPosition(vec3(20.f, 15.f, 0.f));
    node2->addChild(son2);

    //Act
    auto result = *node1 != *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoEqualSonsWithDifferentParents_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("parent1");
    auto son1 = new node("son");
    node1->addChild(son1);

    auto node2 = new node("parent2");
    auto son2 = new node("son");
    node2->addChild(son2);

    //Act
    auto result = *son1 != *son2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoEqualSonsWithDifferentDisplacedPositionParents_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("node");
    node1->setPosition(vec3(2.f, 15.f, 0.f));
    auto son1 = new node("son");
    node1->addChild(son1);

    auto node2 = new node("node");
    node2->setPosition(vec3(20.f, 15.f, 0.f));
    auto son2 = new node("son");
    node2->addChild(son2);

    //Act
    auto result = *son1 != *son2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoEqualSonsWithinDifferentHierarchy_shouldBeDifferent)
{
    //Arrange
    auto parent1 = new node("parent");
    auto son1 = new node("son");
    parent1->addChild(son1);

    auto grandParent = new node("grandParent");
    auto parent2 = new node("parent");
    auto son2 = new node("son");
    parent2->addChild(son2);
    grandParent->addChild(parent2);

    //Act
    auto result = *son1 != *son2;


    //Assert
    ASSERT_TRUE(result);

    safeDelete(grandParent);
    safeDelete(parent1);
}

TEST(node, equals_nodeWithOneComponentAndNodeWithNoComponent_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("node");
    auto node2 = new node("node");

    Mock<component> mock1;
    When(Method(mock1, onNodeChanged)).AlwaysDo([] (node* node) {});
    When(Dtor(mock1)).AlwaysDo([] {});

    auto component1 = &mock1.get();
    node1->addComponent(component1);

    //Act
    auto result = *node1 != *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoNodesWithTheSameComponent_shouldBeEqual)
{
    //Arrange
    auto node1 = new node("node");
    auto node2 = new node("node");

    Mock<component> mock1;
    When(Method(mock1, operator==)).AlwaysReturn(true);
    When(Method(mock1, operator!=)).AlwaysReturn(false);
    When(Method(mock1, onNodeChanged)).AlwaysDo([](node* node) {});
    When(Dtor(mock1)).AlwaysDo([] {});

    auto component1 = &mock1.get();
    node1->addComponent(component1);

    Mock<component> mock2;
    When(Method(mock2, operator==)).AlwaysReturn(true);
    When(Method(mock2, operator!=)).AlwaysReturn(false);
    When(Method(mock2, onNodeChanged)).AlwaysDo([](node* node) {});
    When(Dtor(mock2)).AlwaysDo([] {});

    auto component2 = &mock2.get();
    node2->addComponent(component2);

    //Act
    auto result = *node1 == *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, equals_twoNodesWithDifferentComponents_shouldBeDifferent)
{
    //Arrange
    auto node1 = new node("node");
    auto node2 = new node("node");

    Mock<component> mock1;
    When(Method(mock1, operator==)).AlwaysReturn(false);
    When(Method(mock1, operator!=)).AlwaysReturn(true);
    When(Method(mock1, onNodeChanged)).AlwaysDo([](node* node) {});
    When(Dtor(mock1)).AlwaysDo([] {});

    auto component1 = &mock1.get();
    node1->addComponent(component1);

    Mock<component> mock2;
    When(Method(mock2, operator==)).AlwaysReturn(false);
    When(Method(mock2, operator!=)).AlwaysReturn(true);
    When(Method(mock2, onNodeChanged)).AlwaysDo([](node* node) {});
    When(Dtor(mock2)).AlwaysDo([] {});

    auto component2 = &mock2.get();
    node2->addComponent(component2);

    //Act
    auto result = *node1 != *node2;

    //Assert
    ASSERT_TRUE(result);

    safeDelete(node1);
    safeDelete(node2);
}

TEST(node, clone_twoEmptyNodes_clonedAndOriginalShouldBeEqual)
{
    //Arrange
    auto original = new node("node");

    //Act
    auto cloned = original->clone();

    //Assert
    auto result = *original == *cloned;
    auto pointers = original != cloned;
    ASSERT_TRUE(result);
    ASSERT_TRUE(pointers);

    safeDelete(original);
    safeDelete(cloned);
}

TEST(node, clone_originalAndChangedCloned_clonedAndOriginalShouldBeDifferent)
{
    //Arrange
    auto original = new node("node");

    //Act
    auto cloned = original->clone();
    cloned->setPosition(vec3(10.f, 0.f, 0.f));

    //Assert
    auto result = *original != *cloned;
    auto pointers = original != cloned;
    ASSERT_TRUE(result);
    ASSERT_TRUE(pointers);

    safeDelete(original);
    safeDelete(cloned);
}

TEST(node, clone_originalWithChildAndUnchangedCloned_clonedAndOriginalShouldBeEqual)
{
    //Arrange
    auto original = new node("node");
    auto child = new node("child");
    original->addChild(child);

    //Act
    auto cloned = original->clone();

    //Assert
    auto result = *original == *cloned;
    auto pointers = original != cloned;
    ASSERT_TRUE(result);
    ASSERT_TRUE(pointers);

    safeDelete(original);
    safeDelete(cloned);
}

TEST(node, clone_originalWithChildAndChangedClonedChild_clonedAndOriginalShouldBeDifferent)
{
    //Arrange
    auto original = new node("node");
    auto child = new node("child");
    original->addChild(child);

    //Act
    auto cloned = original->clone();
    cloned->getChildren()->at(0)->setSize(vec3(2.f, 2.f, 2.f));

    //Assert
    auto result = *original != *cloned;
    auto pointers = original != cloned;
    ASSERT_TRUE(result);
    ASSERT_TRUE(pointers);

    safeDelete(original);
    safeDelete(cloned);
}

TEST(node, clone_originalWithChangedChildAndUnchangedCloned_clonedAndOriginalShouldBeEqual)
{
    //Arrange
    auto original = new node("node");
    auto child = new node("child");
    child->setPosition(vec3(0.f, 12.f, 3.f));
    original->addChild(child);

    //Act
    auto cloned = original->clone();

    //Assert
    auto result = *original == *cloned;
    auto pointers = original != cloned;
    ASSERT_TRUE(result);
    ASSERT_TRUE(pointers);

    safeDelete(original);
    safeDelete(cloned);
}

TEST(node, clone_originalWithComponentsAndUchangedCloned_clonedAndOriginalShouldBeEqual)
{
    //Arrange
    auto original = new node("node");

    Mock<component> clonedMock;
    When(Method(clonedMock, operator==)).AlwaysReturn(true);
    When(Method(clonedMock, operator!=)).AlwaysReturn(false);
    When(Method(clonedMock, onNodeChanged)).AlwaysDo([](node* node) {});
    When(Dtor(clonedMock)).AlwaysDo([] {});
    
    Mock<component> mock1;
    When(Method(mock1, operator==)).AlwaysReturn(true);
    When(Method(mock1, operator!=)).AlwaysReturn(false);
    When(Method(mock1, clone)).AlwaysReturn(&clonedMock.get());
    When(Method(mock1, onNodeChanged)).AlwaysDo([](node* node) {});
    When(Dtor(mock1)).AlwaysDo([] {});

    auto component1 = &mock1.get();
    original->addComponent(component1);

    auto child = new node("child");
    child->setPosition(vec3(0.f, 12.f, 3.f));
    original->addChild(child);

    //Act
    auto cloned = original->clone();

    //Assert
    auto result = *original == *cloned;
    auto pointers = original != cloned;
    ASSERT_TRUE(result);
    ASSERT_TRUE(pointers);

    safeDelete(original);
    safeDelete(cloned);
}
#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <core\model.h>
#include <layers\nodeSelection\selectionInputController.h>

using namespace phi;

namespace selectionInputControllerTests
{
    node* newModelNode(string name)
    {
        auto newNode = new node(name);
        newNode->addComponent(new model());
        return newNode;
    }

    TEST(selectionInputController, select_unselectedNode_selectTheNode)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto node = newModelNode("node");
        auto eventRaised = false;
        sceneRoot->addChild(node);

        auto selectionController = selectionInputController(new commandsManager());

        node->selectionChanged.assign([&](phi::node* node) { eventRaised = true; });

        //Act
        selectionController.select(node);

        //Assert
        ASSERT_TRUE(node->isSelected());
        ASSERT_TRUE(eventRaised);

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, deselectAll_selectedNode_deselectsNode)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto node = newModelNode("node");
        sceneRoot->addChild(node);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(node);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        node->selectionChanged.assign([&](phi::node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(node->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_nodeToSelectHasChild_selectsTheParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(parent);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, deselectAll_nodeToSelectHasChild_leavesParentAndChildUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = new node("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_nodeToSelectHasParent_selectsTheParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, deselectAll_nodeToSelectHasParent_leavesParentAndChildUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_nodeToSelectHasSelectedParent_selectsChildAndUnselectParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        parent->addComponent(new model());
        auto child = new node("child");
        child->addComponent(new model());

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_TRUE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, deselectAll_nodeToSelectHasSelectedParent_leavesParentAndChildUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);
        selectionController.select(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_nodeToSelectHasUnselectedGrandparent_selectsGrandparent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = new node("parent");
        auto child = new node("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        auto selectionController = selectionInputController(new commandsManager());

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(grandparent->isSelected());
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, deselectAll_selectedGrandparent_leavesAllUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = new node("parent");
        auto child = new node("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(grandparent->isSelected());
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_nodeToSelectHasSelectedGrandparent_selectsParentAndUnselectGrandparent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(grandparent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(grandparent->isSelected());
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_nodeToSelectHasUnselectedGrandparentAndSelectedParent_selectsChildAndUnselectsParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(grandparent);
        selectionController.select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(grandparent->isSelected());
        ASSERT_FALSE(parent->isSelected());
        ASSERT_TRUE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, execute_nodeToSelectHasSelectedBrother_selectesNode)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child1 = newModelNode("child1");
        auto child2 = newModelNode("child2");

        parent->addChild(child1);
        parent->addChild(child2);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);
        selectionController.select(child1);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child2);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_TRUE(child2->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, executeUndo_nodeToSelectHasSelectedBrother_leavesTreeUntouched)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child1 = newModelNode("child1");
        auto child2 = newModelNode("child2");

        parent->addChild(child1);
        parent->addChild(child2);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);
        selectionController.select(child1);
        selectionController.select(child2);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_FALSE(child2->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, select_groupOfNodeToSelectHasBrotherGroupContainingOneSelectedNode_selectsBrotherGroup)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto group1 = new node("group1");
        auto child1 = new node("child1");
        auto child2 = newModelNode("child2");
        auto group2 = newModelNode("group2");
        auto child3 = new node("child3");
        auto child4 = new node("child4");

        group1->addChild(child1);
        group1->addChild(child2);

        group2->addChild(child3);
        group2->addChild(child4);

        parent->addChild(group1);
        parent->addChild(group2);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);
        selectionController.select(group1);
        selectionController.select(child2);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child3);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(group1->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_FALSE(child2->isSelected());
        ASSERT_TRUE(group2->isSelected());
        ASSERT_FALSE(child3->isSelected());
        ASSERT_FALSE(child4->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, deselectAll_selectedGroup_deselectsAll)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto group1 = new node("group1");
        auto child1 = new node("child1");
        auto child2 = newModelNode("child2");
        auto group2 = newModelNode("group2");
        auto child3 = new node("child3");
        auto child4 = new node("child4");

        group1->addChild(child1);
        group1->addChild(child2);

        group2->addChild(child3);
        group2->addChild(child4);

        parent->addChild(group1);
        parent->addChild(group2);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);
        selectionController.select(group1);
        selectionController.select(child2);
        selectionController.select(child3);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(group1->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_FALSE(child2->isSelected());
        ASSERT_FALSE(group2->isSelected());
        ASSERT_FALSE(child3->isSelected());
        ASSERT_FALSE(child4->isSelected());

        safeDelete(sceneRoot);
    }

    TEST(selectionInputController, execute_nodeToSelectHasSelectedParentButDoesNotHaveModelComponent_leavesTreeUntouched)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = new node("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        auto selectionController = selectionInputController(new commandsManager());
        selectionController.select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        selectionController.select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }
}
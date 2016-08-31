#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <core\model.h>
//#include <context\selectNodeCommand.h>

using namespace phi;

//namespace selectNodeCommandTests
//{
//    node* newModelNode(string name)
//    {
//        auto newNode = new node(name);
//        newNode->addComponent(new model());
//        return newNode;
//    }
//
//    TEST(selectNodeCommand, execute_unselectedNode_selectTheNode)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto nodeToSelect = newModelNode("node");
//        auto eventRaised = false;
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaised = true; });
//
//        sceneRoot->addChild(nodeToSelect);
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_TRUE(nodeToSelect->isSelected());
//        ASSERT_TRUE(eventRaised);
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_unselectedNode_leavesNodeUnselected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto nodeToSelect = newModelNode("node");
//
//        sceneRoot->addChild(nodeToSelect);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 2u;
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasChild_selectsTheParent)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto nodeToSelect = new node("parent");
//        auto child = newModelNode("child");
//
//        nodeToSelect->addChild(child);
//        sceneRoot->addChild(nodeToSelect);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 1u;
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_TRUE(nodeToSelect->isSelected());
//        ASSERT_FALSE(child->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasChild_leavesParentAndChildUnselected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto nodeToSelect = new node("parent");
//        nodeToSelect->addComponent(new model());
//        auto child = new node("child");
//
//        nodeToSelect->addChild(child);
//        sceneRoot->addChild(nodeToSelect);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 2u;
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(nodeToSelect->isSelected());
//        ASSERT_FALSE(child->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasParent_selectsTheParentAndTheChild)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        auto nodeToSelect = new node("child");
//        nodeToSelect->addComponent(new model());
//
//        parent->addChild(nodeToSelect);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 2u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasParent_leavesParentAndChildUnselected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        auto nodeToSelect = new node("child");
//        nodeToSelect->addComponent(new model());
//
//        parent->addChild(nodeToSelect);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 4u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_FALSE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasSelectedParent_selectsChildAndUnselectParent)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//        nodeToSelect->addComponent(new model());
//
//        parent->select();
//        nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 3u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasSelectedParent_leavesParentSelectedAndChildUnselected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//        nodeToSelect->addComponent(new model());
//
//        parent->select();
//        nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 6u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasUnselectedGrandparent_selectsGrandparentTree)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto grandparent = new node("grandparent");
//        grandparent->addComponent(new model());
//        auto parent = new node("parent");
//        auto nodeToSelect = new node("child");
//
//        parent->addChild(nodeToSelect);
//        grandparent->addChild(parent);
//        sceneRoot->addChild(grandparent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 3u;
//        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_TRUE(grandparent->isSelected());
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasUnselectedGrandparent_leavesAllUnselected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto grandparent = new node("grandparent");
//        grandparent->addComponent(new model());
//        auto parent = new node("parent");
//        auto nodeToSelect = new node("child");
//
//        parent->addChild(nodeToSelect);
//        grandparent->addChild(parent);
//        sceneRoot->addChild(grandparent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 6u;
//        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs;
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(grandparent->isSelected());
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_FALSE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasSelectedGrandparent_selectsParentTreeAndUnselectGrandparent)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto grandparent = new node("grandparent");
//        grandparent->addComponent(new model());
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//
//        grandparent->select();
//        parent->select();
//        nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        grandparent->addChild(parent);
//        sceneRoot->addChild(grandparent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 5u;
//        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { grandparent, parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(grandparent->isSelected());
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasSelectedGrandparent_leavesGrandparentTreeSelected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto grandparent = new node("grandparent");
//        grandparent->addComponent(new model());
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//
//        grandparent->select();
//        parent->select();
//        nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        grandparent->addChild(parent);
//        sceneRoot->addChild(grandparent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 10u;
//        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { grandparent, parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_TRUE(grandparent->isSelected());
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasUnselectedGrandparentAndSelectedParent_selectsChildAndUnselectParent)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto grandparent = new node("grandparent");
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//        nodeToSelect->addComponent(new model());
//
//        parent->select();
//        nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        grandparent->addChild(parent);
//        sceneRoot->addChild(grandparent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 3u;
//        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(grandparent->isSelected());
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasUnselectedGrandparentAndSelectedParent_leavesParentTreeSelected)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto grandparent = new node("grandparent");
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//        nodeToSelect->addComponent(new model());
//
//        parent->select();
//        //nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        grandparent->addChild(parent);
//        sceneRoot->addChild(grandparent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 6u;
//        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(grandparent->isSelected());
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasSelectedBrother_selectesNode)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        auto child1 = new node("child1");
//        child1->addComponent(new model());
//        auto child2 = new node("child2");
//        child2->addComponent(new model());
//
//        child2->select();
//
//        parent->addChild(child1);
//        parent->addChild(child2);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 2u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { child2 };
//
//        //Act
//        auto selectCmd = selectNodeCommand(child1, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_TRUE(child1->isSelected());
//        ASSERT_FALSE(child2->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_nodeToSelectHasSelectedBrother_leavesTreeUntouched)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        auto child1 = new node("child1");
//        child1->addComponent(new model());
//        auto child2 = new node("child2");
//        child2->addComponent(new model());
//
//        child2->select();
//
//        parent->addChild(child1);
//        parent->addChild(child2);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 4u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { child2 };
//
//        //Act
//        auto selectCmd = selectNodeCommand(child1, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_FALSE(child1->isSelected());
//        ASSERT_TRUE(child2->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_groupOfNodeToSelectHasBrotherGroupContainingOneSelectedNode_selectsBrotherGroup)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        auto group1 = new node("group1");
//        auto group2 = new node("group2");
//        group2->addComponent(new model());
//        auto child1 = new node("child1");
//        auto child2 = new node("child2");
//        child2->addComponent(new model());
//        auto child3 = new node("child3");
//        auto child4 = new node("child4");
//
//        group1->addChild(child1);
//        group1->addChild(child2);
//
//        group2->addChild(child3);
//        group2->addChild(child4);
//
//        parent->addChild(group1);
//        parent->addChild(group2);
//        sceneRoot->addChild(parent);
//
//        child2->select();
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 4u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { child2 };
//
//        //Act
//        auto selectCmd = selectNodeCommand(child3, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_FALSE(group1->isSelected());
//        ASSERT_FALSE(child1->isSelected());
//        ASSERT_FALSE(child2->isSelected());
//        ASSERT_TRUE(group2->isSelected());
//        ASSERT_TRUE(child3->isSelected());
//        ASSERT_TRUE(child4->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, executeUndo_groupOfNodeToSelectHasBrotherGroupContainingOneSelectedNode_leavesTreeUntouched)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        auto group1 = new node("group1");
//        auto group2 = new node("group2");
//        group2->addComponent(new model());
//        auto child1 = new node("child1");
//        auto child2 = new node("child2");
//        child2->addComponent(new model());
//        auto child3 = new node("child3");
//        auto child4 = new node("child4");
//
//        group1->addChild(child1);
//        group1->addChild(child2);
//
//        group2->addChild(child3);
//        group2->addChild(child4);
//
//        parent->addChild(group1);
//        parent->addChild(group2);
//        sceneRoot->addChild(parent);
//
//        child2->select();
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 8u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { child2 };
//
//        //Act
//        auto selectCmd = selectNodeCommand(child3, &selectedObjs);
//        selectCmd.execute();
//        selectCmd.executeUndo();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_FALSE(parent->isSelected());
//        ASSERT_FALSE(group1->isSelected());
//        ASSERT_FALSE(child1->isSelected());
//        ASSERT_TRUE(child2->isSelected());
//        ASSERT_FALSE(group2->isSelected());
//        ASSERT_FALSE(child3->isSelected());
//        ASSERT_FALSE(child4->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//
//    TEST(selectNodeCommand, execute_nodeToSelectHasSelectedParentButDoesNotHaveModelComponent_leavesTreeUntouched)
//    {
//        //Arrange
//        auto sceneRoot = new node("root");
//        auto parent = new node("parent");
//        parent->addComponent(new model());
//        auto nodeToSelect = new node("child");
//
//        parent->select();
//        nodeToSelect->select();
//
//        parent->addChild(nodeToSelect);
//        sceneRoot->addChild(parent);
//
//        auto eventRaisedCount = 0u;
//        auto expectedEventRaisedCount = 3u;
//        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//        nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//        vector<node*> selectedObjs = { parent, nodeToSelect };
//
//        //Act
//        auto selectCmd = selectNodeCommand(nodeToSelect, &selectedObjs);
//        selectCmd.execute();
//
//        //Assert
//        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//        ASSERT_TRUE(parent->isSelected());
//        ASSERT_TRUE(nodeToSelect->isSelected());
//
//        safeDelete(sceneRoot);
//    }
//}
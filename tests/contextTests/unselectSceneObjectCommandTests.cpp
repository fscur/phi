#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <context\unselectSceneObjectCommand.h>

using namespace phi;

TEST(unselectSceneObjectCommand, execute_singleSelectedNode_unselectNode)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();

    sceneRoot->addChild(node1);
    node1->setIsSelected(true);

    auto expectedEventRaisedCount = 1u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();

    //Assert
    ASSERT_FALSE(node1->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, executeUndo_singleSelectedNode_leavesNodeSelected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();

    sceneRoot->addChild(node1);
    node1->setIsSelected(true);

    auto expectedEventRaisedCount = 2u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();
    command.executeUndo();

    //Assert
    ASSERT_TRUE(node1->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, execute_singleUnselectedNode_leavesNodeUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();

    sceneRoot->addChild(node1);
    node1->setIsSelected(false);

    auto expectedEventRaisedCount = 0u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();

    //Assert
    ASSERT_FALSE(node1->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, executeUndo_singleUnselectedNode_leavesNodeUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();

    sceneRoot->addChild(node1);
    node1->setIsSelected(false);

    auto expectedEventRaisedCount = 0u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();
    command.executeUndo();

    //Assert
    ASSERT_FALSE(node1->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, execute_multipleSelectedNodes_unselectAll)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();
    auto node2 = new node();
    auto node3 = new node();

    sceneRoot->addChild(node1);
    sceneRoot->addChild(node2);
    sceneRoot->addChild(node3);
    node1->setIsSelected(true);
    node2->setIsSelected(true);
    node3->setIsSelected(true);

    auto expectedEventRaisedCount = 3u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1, node2, node3 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();

    //Assert
    ASSERT_FALSE(node1->getIsSelected());
    ASSERT_FALSE(node2->getIsSelected());
    ASSERT_FALSE(node3->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, execute_multipleUnselectedNodes_leavesAllNodesUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();
    auto node2 = new node();
    auto node3 = new node();

    sceneRoot->addChild(node1);
    sceneRoot->addChild(node2);
    sceneRoot->addChild(node3);
    node1->setIsSelected(false);
    node2->setIsSelected(false);
    node3->setIsSelected(false);

    auto expectedEventRaisedCount = 0u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1, node2, node3 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();

    //Assert
    ASSERT_FALSE(node1->getIsSelected());
    ASSERT_FALSE(node2->getIsSelected());
    ASSERT_FALSE(node3->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, executeUndo_multipleSelectedNodes_leavesAllNodesUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();
    auto node2 = new node();
    auto node3 = new node();

    sceneRoot->addChild(node1);
    sceneRoot->addChild(node2);
    sceneRoot->addChild(node3);
    node1->setIsSelected(true);
    node2->setIsSelected(true);
    node3->setIsSelected(true);

    auto expectedEventRaisedCount = 6u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1, node2, node3 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();
    command.executeUndo();

    //Assert
    ASSERT_TRUE(node1->getIsSelected());
    ASSERT_TRUE(node2->getIsSelected());
    ASSERT_TRUE(node3->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, execute_twoSelectedNodesAndOneUnselected_unselectTwoSelectedAndLeavesUnselectedUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();
    auto node2 = new node();
    auto node3 = new node();

    sceneRoot->addChild(node1);
    sceneRoot->addChild(node2);
    sceneRoot->addChild(node3);
    node1->setIsSelected(true);
    node2->setIsSelected(false);
    node3->setIsSelected(true);

    auto expectedEventRaisedCount = 2u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1, node2, node3 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();

    //Assert
    ASSERT_FALSE(node1->getIsSelected());
    ASSERT_FALSE(node2->getIsSelected());
    ASSERT_FALSE(node3->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, executeUndo_twoSelectedNodesAndOneUnselected_leavesAllNodesUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto node1 = new node();
    auto node2 = new node();
    auto node3 = new node();

    sceneRoot->addChild(node1);
    sceneRoot->addChild(node2);
    sceneRoot->addChild(node3);
    node1->setIsSelected(true);
    node2->setIsSelected(false);
    node3->setIsSelected(true);

    auto expectedEventRaisedCount = 4u;
    auto eventRaisedCount = 0u;
    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { node1, node2, node3 };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();
    command.executeUndo();

    //Assert
    ASSERT_TRUE(node1->getIsSelected());
    ASSERT_FALSE(node2->getIsSelected());
    ASSERT_TRUE(node3->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}

TEST(unselectSceneObjectCommand, execute_selectedParentAndUnselectedChild_leavesBothUnselected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto child = new node("child");

    parent->addChild(child);
    sceneRoot->addChild(parent);
    parent->setIsSelected(true);
    child->setIsSelected(false);

    auto expectedEventRaisedCount = 1u;
    auto eventRaisedCount = 0u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> nodesToUnselect = { parent };

    //Act
    auto command = unselectSceneObjectCommand(nodesToUnselect);
    command.execute();

    //Assert
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_FALSE(child->getIsSelected());
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);

    safeDelete(sceneRoot);
}
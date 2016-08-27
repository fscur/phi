#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
//#include <context\unselectNodeCommand.h>

using namespace phi;

//TEST(unselectSceneObjectCommand, execute_singleSelectedNode_unselectNode)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//
//    sceneRoot->addChild(node1);
//    node1->select();
//
//    auto expectedEventRaisedCount = 1u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//
//    //Assert
//    ASSERT_FALSE(node1->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, executeUndo_singleSelectedNode_leavesNodeSelected)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//
//    sceneRoot->addChild(node1);
//    node1->select();
//
//    auto expectedEventRaisedCount = 2u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//    command.executeUndo();
//
//    //Assert
//    ASSERT_TRUE(node1->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, execute_singleUnselectedNode_leavesNodeUntouched)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//
//    sceneRoot->addChild(node1);
//    node1->deselect();
//
//    auto expectedEventRaisedCount = 0u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//
//    //Assert
//    ASSERT_FALSE(node1->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, executeUndo_singleUnselectedNode_leavesNodeUntouched)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//
//    sceneRoot->addChild(node1);
//    node1->deselect();
//
//    auto expectedEventRaisedCount = 0u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//    command.executeUndo();
//
//    //Assert
//    ASSERT_FALSE(node1->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, execute_multipleSelectedNodes_unselectAll)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//    auto node2 = new node();
//    auto node3 = new node();
//
//    sceneRoot->addChild(node1);
//    sceneRoot->addChild(node2);
//    sceneRoot->addChild(node3);
//    node1->select();
//    node2->select();
//    node3->select();
//
//    auto expectedEventRaisedCount = 3u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1, node2, node3 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//
//    //Assert
//    ASSERT_FALSE(node1->isSelected());
//    ASSERT_FALSE(node2->isSelected());
//    ASSERT_FALSE(node3->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, execute_multipleUnselectedNodes_leavesAllNodesUntouched)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//    auto node2 = new node();
//    auto node3 = new node();
//
//    sceneRoot->addChild(node1);
//    sceneRoot->addChild(node2);
//    sceneRoot->addChild(node3);
//    node1->deselect();
//    node2->deselect();
//    node3->deselect();
//
//    auto expectedEventRaisedCount = 0u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1, node2, node3 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//
//    //Assert
//    ASSERT_FALSE(node1->isSelected());
//    ASSERT_FALSE(node2->isSelected());
//    ASSERT_FALSE(node3->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, executeUndo_multipleSelectedNodes_leavesAllNodesUntouched)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//    auto node2 = new node();
//    auto node3 = new node();
//
//    sceneRoot->addChild(node1);
//    sceneRoot->addChild(node2);
//    sceneRoot->addChild(node3);
//    node1->select();
//    node2->select();
//    node3->select();
//
//    auto expectedEventRaisedCount = 6u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1, node2, node3 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//    command.executeUndo();
//
//    //Assert
//    ASSERT_TRUE(node1->isSelected());
//    ASSERT_TRUE(node2->isSelected());
//    ASSERT_TRUE(node3->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, execute_twoSelectedNodesAndOneUnselected_unselectTwoSelectedAndLeavesUnselectedUntouched)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//    auto node2 = new node();
//    auto node3 = new node();
//
//    sceneRoot->addChild(node1);
//    sceneRoot->addChild(node2);
//    sceneRoot->addChild(node3);
//    node1->select();
//    node2->deselect();
//    node3->select();
//
//    auto expectedEventRaisedCount = 2u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1, node2, node3 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//
//    //Assert
//    ASSERT_FALSE(node1->isSelected());
//    ASSERT_FALSE(node2->isSelected());
//    ASSERT_FALSE(node3->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, executeUndo_twoSelectedNodesAndOneUnselected_leavesAllNodesUntouched)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto node1 = new node();
//    auto node2 = new node();
//    auto node3 = new node();
//
//    sceneRoot->addChild(node1);
//    sceneRoot->addChild(node2);
//    sceneRoot->addChild(node3);
//    node1->select();
//    node2->deselect();
//    node3->select();
//
//    auto expectedEventRaisedCount = 4u;
//    auto eventRaisedCount = 0u;
//    node1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    node3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { node1, node2, node3 };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//    command.executeUndo();
//
//    //Assert
//    ASSERT_TRUE(node1->isSelected());
//    ASSERT_FALSE(node2->isSelected());
//    ASSERT_TRUE(node3->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
//
//TEST(unselectSceneObjectCommand, execute_selectedParentAndUnselectedChild_leavesBothUnselected)
//{
//    //Arrange
//    auto sceneRoot = new node("root");
//    auto parent = new node("parent");
//    auto child = new node("child");
//
//    parent->addChild(child);
//    sceneRoot->addChild(parent);
//    parent->select();
//    child->deselect();
//
//    auto expectedEventRaisedCount = 1u;
//    auto eventRaisedCount = 0u;
//    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//    child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
//
//    vector<node*> nodesToUnselect = { parent };
//
//    //Act
//    auto command = unselectNodeCommand(nodesToUnselect, &nodesToUnselect);
//    command.execute();
//
//    //Assert
//    ASSERT_FALSE(parent->isSelected());
//    ASSERT_FALSE(child->isSelected());
//    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
//
//    safeDelete(sceneRoot);
//}
#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <layers\nodeCreation\deleteNodeCommand.h>

using namespace phi;

TEST(deleteNodeCommand, execute_singleNode_deleteNode)
{
    //Arrange
    auto root = new node("root");
    auto node1 = new node("node");

    root->addChild(node1);

    auto expectedTree = new node("root");

    vector<node*> nodesToDelete = { node1 };

    //Act
    auto deleteCmd = deleteNodeCommand(nodesToDelete);
    deleteCmd.execute();

    //Assert
    auto result = *root == *expectedTree;
    ASSERT_TRUE(result);

    safeDelete(root);
    safeDelete(expectedTree);
}

TEST(deleteSceneObjectCommand, executeUndo_singleNode_leavesTreeUntouched)
{
    //Arrange
    auto root = new node("root");
    auto node1 = new node("node");

    root->addChild(node1);

    auto expectedTree = root->clone();

    vector<node*> nodesToDelete = { node1 };

    //Act
    auto deleteCmd = deleteNodeCommand(nodesToDelete);
    deleteCmd.execute();
    deleteCmd.executeUndo();

    //Assert
    auto result = *root == *expectedTree;
    ASSERT_TRUE(result);

    safeDelete(root);
    safeDelete(expectedTree);
}

TEST(deleteSceneObjectCommand, execute_nodeToDeleteHasParent_deleteNode)
{
    //Arrange
    auto root = new node("root");
    auto parent = new node("parent");
    auto child = new node("child");

    parent->addChild(child);
    root->addChild(parent);

    auto expectedTree = new node("root");
    auto parent1 = new node("parent");

    expectedTree->addChild(parent1);

    vector<node*> nodesToDelete = { child };

    //Act
    auto deleteCmd = deleteNodeCommand(nodesToDelete);
    deleteCmd.execute();

    //Assert
    auto result = *root == *expectedTree;
    ASSERT_TRUE(result);

    safeDelete(root);
    safeDelete(expectedTree);
}

TEST(deleteSceneObjectCommand, executeUndo_nodeToDeleteHasParent_leavesTreeUntouched)
{
    //Arrange
    auto root = new node("root");
    auto parent = new node("parent");
    auto child = new node("child");

    parent->addChild(child);
    root->addChild(parent);

    auto expectedTree = root->clone();

    vector<node*> nodesToDelete = { child };

    //Act
    auto deleteCmd = deleteNodeCommand(nodesToDelete);
    deleteCmd.execute();
    deleteCmd.executeUndo();

    //Assert
    auto result = *root == *expectedTree;
    ASSERT_TRUE(result);

    safeDelete(root);
    safeDelete(expectedTree);
}

TEST(deleteSceneObjectCommand, execute_nodeToDeleteHasParentAndChild_deleteParentAndChild)
{
    //Arrange
    auto root = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto child = new node("child");

    parent->addChild(child);
    grandparent->addChild(parent);
    root->addChild(grandparent);

    auto expectedTree = new node("root");
    auto parent1 = new node("grandparent");

    expectedTree->addChild(parent1);

    vector<node*> nodesToDelete = { parent };

    //Act
    auto deleteCmd = deleteNodeCommand(nodesToDelete);
    deleteCmd.execute();

    //Assert
    auto result = *root == *expectedTree;
    ASSERT_TRUE(result);

    safeDelete(root);
    safeDelete(expectedTree);
}

TEST(deleteSceneObjectCommand, executeUndo_nodeToDeleteHasParentAndChild_leavesTreeUntouched)
{
    //Arrange
    auto root = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto child = new node("child");

    parent->addChild(child);
    grandparent->addChild(parent);
    root->addChild(grandparent);

    auto expectedTree = root->clone();

    vector<node*> nodesToDelete = { parent };

    //Act
    auto deleteCmd = deleteNodeCommand(nodesToDelete);
    deleteCmd.execute();
    deleteCmd.executeUndo();

    //Assert
    auto result = *root == *expectedTree;
    ASSERT_TRUE(result);

    safeDelete(root);
    safeDelete(expectedTree);
}
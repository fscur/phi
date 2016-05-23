#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <scenes\selectSceneObjectCommand.h>

using namespace phi;

TEST(selectSceneObjectCommand, execute_unselectedNode_selectTheNode)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto nodeToSelect = new node("node");
    auto eventRaised = false;
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaised = true; });

    sceneRoot->addChild(nodeToSelect);

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_TRUE(nodeToSelect->getIsSelected());
    ASSERT_TRUE(eventRaised);

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_unselectedNode_leavesNodeUnselected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto nodeToSelect = new node("node");

    sceneRoot->addChild(nodeToSelect);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 2u;
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasChild_selectsTheParentAndTheChild)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto nodeToSelect = new node("parent");
    auto child = new node("child");

    nodeToSelect->addChild(child);
    sceneRoot->addChild(nodeToSelect);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 2u;
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_TRUE(nodeToSelect->getIsSelected());
    ASSERT_TRUE(child->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasChild_leavesParentAndChildUnselected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto nodeToSelect = new node("parent");
    auto child = new node("child");

    nodeToSelect->addChild(child);
    sceneRoot->addChild(nodeToSelect);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 4u;
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(nodeToSelect->getIsSelected());
    ASSERT_FALSE(child->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasParent_selectsTheParentAndTheChild)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->addChild(nodeToSelect);
    sceneRoot->addChild(parent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 2u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_TRUE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasParent_leavesParentAndChildUnselected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->addChild(nodeToSelect);
    sceneRoot->addChild(parent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 4u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_FALSE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasSelectedParent_selectsChildAndUnselectParent)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->setIsSelected(true);
    nodeToSelect->setIsSelected(true);

    parent->addChild(nodeToSelect);
    sceneRoot->addChild(parent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 3u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { parent, nodeToSelect };

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasSelectedParent_leavesParentSelectedAndChildUnselected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->setIsSelected(true);
    nodeToSelect->setIsSelected(true);

    parent->addChild(nodeToSelect);
    sceneRoot->addChild(parent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 6u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { parent, nodeToSelect };

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_TRUE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasUnselectedGrandparent_selectsGrandparentTree)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->addChild(nodeToSelect);
    grandparent->addChild(parent);
    sceneRoot->addChild(grandparent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 3u;
    grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_TRUE(grandparent->getIsSelected());
    ASSERT_TRUE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasUnselectedGrandparent_leavesAllUnselected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->addChild(nodeToSelect);
    grandparent->addChild(parent);
    sceneRoot->addChild(grandparent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 6u;
    grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs;

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(grandparent->getIsSelected());
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_FALSE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasSelectedGrandparent_selectsParentTreeAndUnselectGrandparent)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    grandparent->setIsSelected(true);
    parent->setIsSelected(true);
    nodeToSelect->setIsSelected(true);

    parent->addChild(nodeToSelect);
    grandparent->addChild(parent);
    sceneRoot->addChild(grandparent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 5u;
    grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { grandparent, parent, nodeToSelect };

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(grandparent->getIsSelected());
    ASSERT_TRUE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasSelectedGrandparent_leavesGrandparentTreeSelected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    grandparent->setIsSelected(true);
    parent->setIsSelected(true);
    nodeToSelect->setIsSelected(true);

    parent->addChild(nodeToSelect);
    grandparent->addChild(parent);
    sceneRoot->addChild(grandparent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 10u;
    grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { grandparent, parent, nodeToSelect };

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_TRUE(grandparent->getIsSelected());
    ASSERT_TRUE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasUnselectedGrandparentAndSelectedParent_selectsChildAndUnselectParent)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->setIsSelected(true);
    nodeToSelect->setIsSelected(true);

    parent->addChild(nodeToSelect);
    grandparent->addChild(parent);
    sceneRoot->addChild(grandparent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 3u;
    grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { parent, nodeToSelect };

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(grandparent->getIsSelected());
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasUnselectedGrandparentAndSelectedParent_leavesParentTreeSelected)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto grandparent = new node("grandparent");
    auto parent = new node("parent");
    auto nodeToSelect = new node("child");

    parent->setIsSelected(true);
    nodeToSelect->setIsSelected(true);

    parent->addChild(nodeToSelect);
    grandparent->addChild(parent);
    sceneRoot->addChild(grandparent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 6u;
    grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    nodeToSelect->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { parent, nodeToSelect };

    //Act
    auto selectCmd = selectSceneObjectCommand(nodeToSelect, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(grandparent->getIsSelected());
    ASSERT_TRUE(parent->getIsSelected());
    ASSERT_TRUE(nodeToSelect->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_nodeToSelectHasSelectedBrother_selectesNode)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto child1 = new node("child1");
    auto child2 = new node("child2");

    child2->setIsSelected(true);

    parent->addChild(child1);
    parent->addChild(child2);
    sceneRoot->addChild(parent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 2u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { child2 };

    //Act
    auto selectCmd = selectSceneObjectCommand(child1, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_TRUE(child1->getIsSelected());
    ASSERT_FALSE(child2->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_nodeToSelectHasSelectedBrother_leavesTreeUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto child1 = new node("child1");
    auto child2 = new node("child2");

    child2->setIsSelected(true);

    parent->addChild(child1);
    parent->addChild(child2);
    sceneRoot->addChild(parent);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 4u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { child2 };

    //Act
    auto selectCmd = selectSceneObjectCommand(child1, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_FALSE(child1->getIsSelected());
    ASSERT_TRUE(child2->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, execute_groupOfNodeToSelectHasBrotherGroupContainingOneSelectedNode_selectsBrotherGroup)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto group1 = new node("group1");
    auto group2 = new node("group2");
    auto child1 = new node("child1");
    auto child2 = new node("child2");
    auto child3 = new node("child3");
    auto child4 = new node("child4");
    
    group1->addChild(child1);
    group1->addChild(child2);

    group2->addChild(child3);
    group2->addChild(child4);

    parent->addChild(group1);
    parent->addChild(group2);
    sceneRoot->addChild(parent);

    child2->setIsSelected(true);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 4u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { child2 };

    //Act
    auto selectCmd = selectSceneObjectCommand(child3, selectedObjs);
    selectCmd.execute();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_FALSE(group1->getIsSelected());
    ASSERT_FALSE(child1->getIsSelected());
    ASSERT_FALSE(child2->getIsSelected());
    ASSERT_TRUE(group2->getIsSelected());
    ASSERT_TRUE(child3->getIsSelected());
    ASSERT_TRUE(child4->getIsSelected());

    safeDelete(sceneRoot);
}

TEST(selectSceneObjectCommand, executeUndo_groupOfNodeToSelectHasBrotherGroupContainingOneSelectedNode_leavesTreeUntouched)
{
    //Arrange
    auto sceneRoot = new node("root");
    auto parent = new node("parent");
    auto group1 = new node("group1");
    auto group2 = new node("group2");
    auto child1 = new node("child1");
    auto child2 = new node("child2");
    auto child3 = new node("child3");
    auto child4 = new node("child4");

    group1->addChild(child1);
    group1->addChild(child2);

    group2->addChild(child3);
    group2->addChild(child4);

    parent->addChild(group1);
    parent->addChild(group2);
    sceneRoot->addChild(parent);

    child2->setIsSelected(true);

    auto eventRaisedCount = 0u;
    auto expectedEventRaisedCount = 8u;
    parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
    child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

    vector<node*> selectedObjs = { child2 };

    //Act
    auto selectCmd = selectSceneObjectCommand(child3, selectedObjs);
    selectCmd.execute();
    selectCmd.executeUndo();

    //Assert
    ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
    ASSERT_FALSE(parent->getIsSelected());
    ASSERT_FALSE(group1->getIsSelected());
    ASSERT_FALSE(child1->getIsSelected());
    ASSERT_TRUE(child2->getIsSelected());
    ASSERT_FALSE(group2->getIsSelected());
    ASSERT_FALSE(child3->getIsSelected());
    ASSERT_FALSE(child4->getIsSelected());

    safeDelete(sceneRoot);
}
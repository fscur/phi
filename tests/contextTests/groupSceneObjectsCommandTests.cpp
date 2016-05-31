#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <context\groupSceneObjectsCommand.h>

using namespace phi;

TEST(groupSceneObjectsCommand, execute_singleNodeToBeGrouped_doesNotGroup)
{
    //Arrange
    auto sceneRoot = new node();
    auto son1 = new node();
    sceneRoot->addChild(son1);
    auto expectedTree = sceneRoot->clone();

    vector<node*> nodesToGroup = { son1 };

    //Act
    auto groupCmd = groupSceneObjectsCommand(nodesToGroup);
    groupCmd.execute();

    //Assert
    auto result = *expectedTree == *sceneRoot;
    ASSERT_TRUE(result);

    safeDelete(sceneRoot);
    safeDelete(expectedTree);
}

TEST(groupSceneObjectsCommand, execute_twoNodesToBeGrouped_groupTheTwoNodes)
{
    //Arrange
    auto sceneRoot = new node();
    auto son1 = new node();
    auto son2 = new node();

    sceneRoot->addChild(son1);
    sceneRoot->addChild(son2);
    vector<node*> nodesToGroup = { son1, son2 };

    auto expectedTree = new node();
    auto group1 = new node("group");
    auto expectedSon1 = son1->clone();
    auto expectedSon2 = son2->clone();

    group1->addChild(expectedSon1);
    group1->addChild(expectedSon2);
    expectedTree->addChild(group1);

    //Act
    auto groupCmd = groupSceneObjectsCommand(nodesToGroup);
    groupCmd.execute();

    //Assert
    auto result = *expectedTree == *sceneRoot;
    ASSERT_TRUE(result);

    safeDelete(sceneRoot);
    safeDelete(expectedTree);
}

TEST(groupSceneObjectsCommand, executeUndo_twoNodesToBeGrouped_leavesTheTreeUntouched)
{
    //Arrange
    auto sceneRoot = new node();
    auto son1 = new node();
    auto son2 = new node();

    sceneRoot->addChild(son1);
    sceneRoot->addChild(son2);
    vector<node*> nodesToGroup = { son1, son2 };

    auto expectedTree = sceneRoot->clone();

    //Act
    auto groupCmd = groupSceneObjectsCommand(nodesToGroup);
    groupCmd.execute();
    groupCmd.executeUndo();

    //Assert
    auto result = *expectedTree == *sceneRoot;
    ASSERT_TRUE(result);

    safeDelete(sceneRoot);
    safeDelete(expectedTree);
}

TEST(groupSceneObjectsCommand, execute_fiveNodesToBeGrouped_groupTheFiveNodes)
{
    //Arrange
    auto sceneRoot = new node();
    auto son1 = new node();
    auto son2 = new node();
    auto son3 = new node();
    auto son4 = new node();
    auto son5 = new node();

    sceneRoot->addChild(son1);
    sceneRoot->addChild(son2);
    sceneRoot->addChild(son3);
    sceneRoot->addChild(son4);
    sceneRoot->addChild(son5);

    vector<node*> nodesToGroup = { son1, son2, son3, son4, son5 };

    auto expectedTree = new node();
    auto group1 = new node("group");
    auto expectedSon1 = son1->clone();
    auto expectedSon2 = son2->clone();
    auto expectedSon3 = son3->clone();
    auto expectedSon4 = son4->clone();
    auto expectedSon5 = son5->clone();

    group1->addChild(expectedSon1);
    group1->addChild(expectedSon2);
    group1->addChild(expectedSon3);
    group1->addChild(expectedSon4);
    group1->addChild(expectedSon5);
    expectedTree->addChild(group1);

    //Act
    auto groupCmd = groupSceneObjectsCommand(nodesToGroup);
    groupCmd.execute();

    //Assert
    auto result = *expectedTree == *sceneRoot;
    ASSERT_TRUE(result);

    safeDelete(sceneRoot);
    safeDelete(expectedTree);
}

TEST(groupSceneObjectsCommand, executeUndo_fiveNodesToBeGrouped_leavesTheTreeUntouched)
{
    //Arrange
    auto sceneRoot = new node();
    auto son1 = new node();
    auto son2 = new node();
    auto son3 = new node();
    auto son4 = new node();
    auto son5 = new node();

    sceneRoot->addChild(son1);
    sceneRoot->addChild(son2);
    sceneRoot->addChild(son3);
    sceneRoot->addChild(son4);
    sceneRoot->addChild(son5);
    vector<node*> nodesToGroup = { son1, son2, son3, son4, son5 };

    auto expectedTree = sceneRoot->clone();

    //Act
    auto groupCmd = groupSceneObjectsCommand(nodesToGroup);
    groupCmd.execute();
    groupCmd.executeUndo();

    //Assert
    auto result = *expectedTree == *sceneRoot;
    ASSERT_TRUE(result);

    safeDelete(sceneRoot);
    safeDelete(expectedTree);
}

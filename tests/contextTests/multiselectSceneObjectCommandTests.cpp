#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <scenes\multiselectSceneObjectCommand.h>

using namespace phi;

TEST(multiselectSceneObjectCommand, execute_twoNodesAndOneAlreadySelected_selectsBothNodes)
{
    //Arrange
    auto root = new node("root");
    auto son1 = new node("son1");
    auto son2 = new node("son2");

    son1->setIsSelected(true);

    //Act
    auto multiselectCmd = multiselectSceneObjectCommand(son2);
    multiselectCmd.execute();

    //Assert
    ASSERT_TRUE(son1->getIsSelected());
    ASSERT_TRUE(son2->getIsSelected());

    safeDelete(root);
}
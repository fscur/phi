#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <layers\nodeSelection\multiselectNodeCommand.h>

using namespace phi;

TEST(multiselectNodeCommand, execute_twoNodesAndOneAlreadySelected_selectsBothNodes)
{
    //Arrange
    auto root = new node("root");
    auto son1 = new node("son1");
    auto son2 = new node("son2");

    son1->select();

    //Act
    auto multiselectCmd = multiselectNodeCommand(son2);
    multiselectCmd.execute();

    //Assert
    ASSERT_TRUE(son1->isSelected());
    ASSERT_TRUE(son2->isSelected());

    safeDelete(root);
}
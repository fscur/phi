#include <precompiled.h>
#include <gtest\gtest.h>

#pragma warning(disable : 4100)
#include <fakeit\fakeit.hpp>

#include <core\command.h>
#include <core\multiCommand.h>

using namespace fakeit;
using namespace phi;

TEST(multiCommand, execute_twoCommands_executeTheTwoCommands)
{
    //Arrange
    auto expectedFirstCommandExecutionCount = 1u;
    auto expectedSecondCommandExecutionCount = 1u;
    auto firstCommandExecutionCount = 0u;
    auto secondCommandExecutionCount = 0u;

    auto expectedFirstCommandUndoCount = 0u;
    auto expectedSecondCommandUndoCount = 0u;
    auto firstCommandUndoCount = 0u;
    auto secondCommandUndoCount = 0u;

    Mock<command> mock1;
    When(Dtor(mock1)).AlwaysDo([] {});
    When(Method(mock1, execute)).AlwaysDo([&] { firstCommandExecutionCount++; });
    When(Method(mock1, executeUndo)).AlwaysDo([&] { firstCommandUndoCount++; });

    auto firstCommand = &mock1.get();

    Mock<command> mock2;
    When(Dtor(mock2)).AlwaysDo([] {});
    When(Method(mock2, execute)).AlwaysDo([&] { secondCommandExecutionCount++; });
    When(Method(mock2, executeUndo)).AlwaysDo([&] { secondCommandUndoCount++; });

    auto secondCommand = &mock2.get();

    auto command = multiCommand(
    {
        firstCommand,
        secondCommand
    });

    //Act
    command.execute();

    //Assert
    ASSERT_EQ(expectedFirstCommandExecutionCount, firstCommandExecutionCount);
    ASSERT_EQ(expectedFirstCommandUndoCount, firstCommandUndoCount);

    ASSERT_EQ(expectedSecondCommandExecutionCount, secondCommandExecutionCount);
    ASSERT_EQ(expectedSecondCommandUndoCount, secondCommandUndoCount);
}

TEST(multiCommand, executeUndo_twoCommands_undoTheTwoCommandsInTheReverseOrder)
{
    //Arrange
    auto expectedFirstCommandExecutionCount = 0u;
    auto expectedSecondCommandExecutionCount = 0u;
    auto firstCommandExecutionCount = 0u;
    auto secondCommandExecutionCount = 0u;

    auto expectedFirstCommandUndoCount = 1u;
    auto expectedSecondCommandUndoCount = 1u;
    auto firstCommandUndoCount = 0u;
    auto secondCommandUndoCount = 0u;

    auto expectedLastCommand = "first";
    auto lastCommand = "";

    Mock<command> mock1;
    When(Dtor(mock1)).AlwaysDo([] {});
    When(Method(mock1, execute)).AlwaysDo([&] { firstCommandExecutionCount++; });
    When(Method(mock1, executeUndo)).AlwaysDo([&] 
    { 
        firstCommandUndoCount++;
        lastCommand = "first";
    });

    auto firstCommand = &mock1.get();

    Mock<command> mock2;
    When(Dtor(mock2)).AlwaysDo([] {});
    When(Method(mock2, execute)).AlwaysDo([&] { secondCommandExecutionCount++; });
    When(Method(mock2, executeUndo)).AlwaysDo([&] 
    {
        secondCommandUndoCount++;
        lastCommand = "second";
    });

    auto secondCommand = &mock2.get();

    auto command = multiCommand(vector<phi::command*>
    {
        firstCommand,
        secondCommand
    });

    //Act
    command.executeUndo();

    //Assert
    ASSERT_EQ(expectedFirstCommandExecutionCount, firstCommandExecutionCount);
    ASSERT_EQ(expectedFirstCommandUndoCount, firstCommandUndoCount);

    ASSERT_EQ(expectedSecondCommandExecutionCount, secondCommandExecutionCount);
    ASSERT_EQ(expectedSecondCommandUndoCount, secondCommandUndoCount);

    ASSERT_EQ(expectedLastCommand, lastCommand);
}
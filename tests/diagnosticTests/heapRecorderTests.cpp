#include <precompiled.h>
#include <diagnostic\memoryTracker\heapRecorder.h>
#include <diagnostic\memoryTracker\allocation.h>
#include <diagnostic\memoryTracker\deallocation.h>

#include <gtest\gtest.h>

using namespace phi;

TEST(heapRecorder, heap_OneAllocationAndOneDelete_ReturnZeroMemoryLeaksAndZeroUnnecessaryDeallocations)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    recorder.registerAllocation(address);
    recorder.registerDeallocation(address);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_FALSE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_OneAllocation_ReturnsOneMemoryLeak)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    recorder.registerAllocation(address);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_TRUE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
    ASSERT_EQ(memoryLeaks[0]->address, address);
}

TEST(heapRecorder, heap_DeallocationWithoutAllocation_ReturnsOneUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    recorder.registerDeallocation(address);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_TRUE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 1u);
}

TEST(heapRecorder, heap_OneAllocationAndTwoDeletes_ReturnsOneUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    recorder.registerAllocation(address);
    recorder.registerDeallocation(address);
    recorder.registerDeallocation(address);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_TRUE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 1u);
}

TEST(heapRecorder, heap_TwoAllocationsAndTwoDeletesOfDifferentAddresses_ReturnsZeroMemoryLeaksAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address1 = reinterpret_cast<void*>(1);
    auto address2 = reinterpret_cast<void*>(2);

    //Act
    recorder.registerAllocation(address1);
    recorder.registerDeallocation(address1);
    recorder.registerAllocation(address2);
    recorder.registerDeallocation(address2);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_FALSE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_AllocationDeallocationAllocationOfTheSameAddress_ReturnsOneMemoryLeaksAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    recorder.registerAllocation(address);
    recorder.registerDeallocation(address);
    recorder.registerAllocation(address);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_TRUE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
    ASSERT_EQ(memoryLeaks[0]->address, address);
}

TEST(heapRecorder, heap_AllocationDeallocationAllocationDeallocationOfTheSameAddress_ReturnsZeroMemoryLeaksAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    recorder.registerAllocation(address);
    recorder.registerDeallocation(address);
    recorder.registerAllocation(address);
    recorder.registerDeallocation(address);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_FALSE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_ThreeAllocationsAndTwoDeallocations_ReturnsOneMemoryLeakAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address1 = reinterpret_cast<void*>(1);
    auto address2 = reinterpret_cast<void*>(2);
    auto address3 = reinterpret_cast<void*>(3);

    //Act
    recorder.registerAllocation(address1);
    recorder.registerDeallocation(address1);

    recorder.registerAllocation(address2);

    recorder.registerAllocation(address3);
    recorder.registerDeallocation(address3);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_TRUE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
    ASSERT_EQ(memoryLeaks[0]->address, address2);
}

TEST(heapRecorder, heap_ThreeAllocationsAndThreeDeallocations_ReturnsOneMemoryLeakAndOneUnnecessaryDeallocation)
{
    //Arrange
    auto recorder = heapRecorder<allocation, deallocation>();
    auto address1 = reinterpret_cast<void*>(1);
    auto address2 = reinterpret_cast<void*>(2);
    auto address3 = reinterpret_cast<void*>(3);

    //Act
    recorder.registerAllocation(address1);
    recorder.registerDeallocation(address1);

    recorder.registerAllocation(address2);

    recorder.registerAllocation(address3);
    recorder.registerDeallocation(address3);

    recorder.registerDeallocation(address1);

    //Assert
    auto memoryLeaks = recorder.getMemoryLeaks();
    auto redundantDeallocations = recorder.getRedundantDeallocations();

    ASSERT_TRUE(recorder.hasMemoryIssues());
    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 1u);
    ASSERT_EQ(memoryLeaks[0]->address, address2);
}

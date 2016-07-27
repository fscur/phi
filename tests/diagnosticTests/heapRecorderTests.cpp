#include <precompiled.h>
#include <diagnostic\memoryTracker\heapRecorder.h>
#include <diagnostic\memoryTracker\allocation.h>
#include <diagnostic\memoryTracker\deallocation.h>

#include <gtest\gtest.h>

using namespace phi;

TEST(heapRecorder, heap_OneAllocationAndOneDelete_ReturnZeroMemoryLeaksAndZeroUnnecessaryDeallocations)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    heap.registerAllocation(address);
    heap.registerDeallocation(address);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_OneAllocation_ReturnsOneMemoryLeak)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    heap.registerAllocation(address);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_DeallocationWithoutAllocation_ReturnsOneUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    heap.registerDeallocation(address);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 1u);
}

TEST(heapRecorder, heap_OneAllocationAndTwoDeletes_ReturnsOneUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    heap.registerAllocation(address);
    heap.registerDeallocation(address);
    heap.registerDeallocation(address);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 1u);
}

TEST(heapRecorder, heap_TwoAllocationsAndTwoDeletesOfDifferentAddresses_ReturnsZeroMemoryLeaksAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address1 = reinterpret_cast<void*>(1);
    auto address2 = reinterpret_cast<void*>(2);

    //Act
    heap.registerAllocation(address1);
    heap.registerDeallocation(address1);
    heap.registerAllocation(address2);
    heap.registerDeallocation(address2);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_AllocationDeallocationAllocationOfTheSameAddress_ReturnsOneMemoryLeaksAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    heap.registerAllocation(address);
    heap.registerDeallocation(address);
    heap.registerAllocation(address);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_AllocationDeallocationAllocationDeallocationOfTheSameAddress_ReturnsZeroMemoryLeaksAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address = reinterpret_cast<void*>(1);

    //Act
    heap.registerAllocation(address);
    heap.registerDeallocation(address);
    heap.registerAllocation(address);
    heap.registerDeallocation(address);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 0u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_ThreeAllocationsAndTwoDeallocations_ReturnsOneMemoryLeakAndZeroUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address1 = reinterpret_cast<void*>(1);
    auto address2 = reinterpret_cast<void*>(2);
    auto address3 = reinterpret_cast<void*>(3);

    //Act
    heap.registerAllocation(address1);
    heap.registerDeallocation(address1);

    heap.registerAllocation(address2);

    heap.registerAllocation(address3);
    heap.registerDeallocation(address3);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 0u);
}

TEST(heapRecorder, heap_ThreeAllocationsAndThreeDeallocations_ReturnsOneMemoryLeakAndOneUnnecessaryDeallocation)
{
    //Arrange
    auto heap = heapRecorder<allocation, deallocation>();
    auto address1 = reinterpret_cast<void*>(1);
    auto address2 = reinterpret_cast<void*>(2);
    auto address3 = reinterpret_cast<void*>(3);

    //Act
    heap.registerAllocation(address1);
    heap.registerDeallocation(address1);

    heap.registerAllocation(address2);

    heap.registerAllocation(address3);
    heap.registerDeallocation(address3);

    heap.registerDeallocation(address1);

    //Assert
    auto memoryLeaks = heap.getMemoryLeaks();
    auto redundantDeallocations = heap.getRedundantDeallocations();

    ASSERT_EQ(memoryLeaks.size(), 1u);
    ASSERT_EQ(redundantDeallocations.size(), 1u);
}

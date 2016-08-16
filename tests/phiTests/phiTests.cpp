#include <precompiled.h>
#include <phi.h>

#include <gtest\gtest.h>
#include <fakeit\fakeit.hpp>

using namespace phi;

class mapContainsFixture : public testing::Test
{
public:
    map<int, string> map;
public:
    void SetUp() 
    {
        map[0] = "zero";
        map[5] = "five";
        map[2] = "two";
        map[12] = "twelve";
        map[1] = "one";
    }

    void TearDown() 
    {
    }
};

class vectorFixture :
    public testing::Test
{
public:
    vector<int> vector;
public:
    void SetUp()
    {
        vector.push_back(5);
        vector.push_back(1984);
        vector.push_back(0);
        vector.push_back(-2877);
        vector.push_back(26);
        vector.push_back(38);
    }

    void TearDown()
    {
    }
};

TEST_F(mapContainsFixture, contains_mapContainingValue_returnsTrue)
{
    auto value = 5;

    auto result = phi::contains(map, value);
    ASSERT_TRUE(result);
}

TEST_F(mapContainsFixture, contains_mapNotContainingValue_returnsFalse)
{
    auto value = -13;

    auto result = phi::contains(map, value);
    ASSERT_FALSE(result);
}

TEST_F(vectorFixture, contains_vectorContainingValue_returnsTrue)
{
    auto value = 1984;

    auto result = phi::contains(vector, value);
    ASSERT_TRUE(result);
}

TEST_F(vectorFixture, contains_vectorNotContainingValue_returnsFalse)
{
    auto value = -8;

    auto result = phi::contains(vector, value);
    ASSERT_FALSE(result);
}

TEST_F(vectorFixture, removeIfContains_vectorContainingValue_RemoveValueFromVector)
{
    auto valueToRemove = 1984;

    phi::removeIfContains(vector, valueToRemove);

    auto result = phi::contains(vector, valueToRemove);
    ASSERT_FALSE(result);
}

TEST_F(vectorFixture, removeIfContains_vectorNotContainingValue_VectorRemainsUntouched)
{
    auto valueToRemove = 33;

    auto vectorValues = phi::vector<int>();
    for (auto value : vector)
        vectorValues.push_back(value);

    phi::removeIfContains(vector, valueToRemove);

    for(auto value : vectorValues)
        ASSERT_TRUE(phi::contains(vector, value));
}

TEST_F(vectorFixture, getLastElementOf_vectorContainingValue_ReturnsLastElement)
{
    //Arrange & Act
    auto result = phi::getLastElementOf(vector);

    //Assert
    auto expected = 38;
    ASSERT_EQ(expected, result);
}

TEST(phi, getLastElementOf_emptyVector_ThrowsException)
{
    //Arrange
    auto vector = phi::vector<int>();

    //Act and Assert
    ASSERT_THROW(phi::getLastElementOf(vector), std::exception);
}

TEST(phi, indexOf_vectorContainingValue_ReturnsValueIndex)
{
    //Arrange
    auto v = vector<int>();
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    //Act
    auto result = phi::indexOf(v, 2);

    //Assert
    auto expected = 2u;
    ASSERT_EQ(expected, result);
}

TEST(phi, indexOf_vectorNotContainingValue_ThrowsException)
{
    //Arrange
    auto v = vector<int>();
    v.push_back(0);
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    //Act & Assert
    ASSERT_THROW(phi::indexOf(v, 5), std::exception);
}

TEST(phi, indexOf_vectorPtrContainingValue_ReturnsValueIndex)
{
    //Arrange
    auto v = new vector<int>();
    v->push_back(0);
    v->push_back(1);
    v->push_back(2);
    v->push_back(3);

    //Act
    auto result = phi::indexOf(v, 2);

    //Assert
    auto expected = 2u;
    ASSERT_EQ(expected, result);

    safeDelete(v);
}

TEST(phi, indexOf_vectorPtrContainingValuePtr_ReturnsValuePtrIndex)
{
    //Arrange
    auto zero = new int(0);
    auto one = new int(1);
    auto two = new int(2);
    auto three = new int(3);

    auto v = new vector<int*>();
    v->push_back(zero);
    v->push_back(one);
    v->push_back(two);
    v->push_back(three);

    //Act
    auto result = phi::indexOf(v, three);

    //Assert
    auto expected = 3u;
    ASSERT_EQ(expected, result);

    safeDelete(zero);
    safeDelete(one);
    safeDelete(two);
    safeDelete(three);
    safeDelete(v);
}
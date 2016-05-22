#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\transform.h>

using namespace phi;

TEST(transform, equals_twoEmptyTransforms_returnsTrue)
{
    //Arrange
    auto transform1 = transform();
    auto transform2 = transform();

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDisplacedLocalPositionTransforms_returnsTrue)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalPosition(vec3(1.f, 0.f, 15.f));
    auto transform2 = transform();
    transform2.setLocalPosition(vec3(1.f, 0.f, 15.f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDifferentDisplacedLocalPositionTransforms_returnsFalse)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalPosition(vec3(1.f, 4.f, 23.f));
    auto transform2 = transform();
    transform2.setLocalPosition(vec3(1.f, 0.f, 15.f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_displacedLocalPositionTransformAndEmptyTransform_returnsFalse)
{
    //Arrange
    auto transform1 = transform();
    auto transform2 = transform();
    transform2.setLocalPosition(vec3(1.f, 0.f, 15.f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_twoDisplacedLocalSizeTransforms_returnsTrue)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalSize(vec3(2.f, 2.f, 2.f));
    auto transform2 = transform();
    transform2.setLocalSize(vec3(2.f, 2.f, 2.f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDifferentDisplacedLocalSizeTransforms_returnsFalse)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalSize(vec3(4.f, 4.f, 3.f));
    auto transform2 = transform();
    transform2.setLocalSize(vec3(2.f, 2.f, 2.f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_twoDisplacedLocalOrientationTransforms_returnsTrue)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalOrientation(quat(vec3(0.f, 0.f, 0.707f)));
    auto transform2 = transform();
    transform2.setLocalOrientation(quat(vec3(0.f, 0.f, 0.707f)));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDifferentDisplacedLocalOrientationTransforms_returnsFalse)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalOrientation(quat(vec3(1.101f, 0.f, 0.f)));
    auto transform2 = transform();
    transform2.setLocalOrientation(quat(vec3(0.f, 0.f, 1.58f)));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_twoDisplacedDirectionTransforms_returnsTrue)
{
    //Arrange
    auto transform1 = transform();
    transform1.setDirection(vec3(0.f, 2.65f, 1.f));
    auto transform2 = transform();
    transform2.setDirection(vec3(0.f, 2.65f, 1.f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDifferentDisplacedDirectionTransforms_returnsFalse)
{
    //Arrange
    auto transform1 = transform();
    transform1.setDirection(vec3(1.f, 0.f, 1.f));
    auto transform2 = transform();
    transform2.setDirection(vec3(0.f, 0.f, 1.58f));

    //Act
    auto result = transform1 == transform2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_twoEmptyTransformsWithEmptyParents_returnsTrue)
{
    //Arrange
    auto parent1 = transform();
    auto child1 = transform();
    child1.setParent(&parent1);

    auto parent2 = transform();
    auto child2 = transform();
    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoEmptyTransformsWithDisplacedPositionParents_returnsTrue)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalPosition(vec3(15.f, 0.f, 12.f));
    auto child1 = transform();
    child1.setParent(&parent1);

    auto parent2 = transform();
    parent2.setLocalPosition(vec3(15.f, 0.f, 12.f));
    auto child2 = transform();
    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoEmptyTransformsWithDifferentDisplacedPositionParents_returnsFalse)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalPosition(vec3(20.f, 0.f, 2.f));
    auto child1 = transform();
    child1.setParent(&parent1);

    auto parent2 = transform();
    parent2.setLocalPosition(vec3(15.f, 0.f, 12.f));
    auto child2 = transform();
    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_twoDisplacedPositionTransformsWithDisplacedPositionParents_returnsTrue)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalPosition(vec3(15.f, 0.f, 12.f));
    auto child1 = transform();
    child1.setLocalPosition(vec3(2.f, 0.f, 0.f));
    child1.setParent(&parent1);

    auto parent2 = transform();
    parent2.setLocalPosition(vec3(15.f, 0.f, 12.f));
    auto child2 = transform();
    child2.setLocalPosition(vec3(2.f, 0.f, 0.f));
    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDisplacedTransformsWithDisplacedParents_returnsTrue)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalPosition(vec3(20.f, 0.f, 2.f));
    parent1.setLocalSize(vec3(2.f, 2.f, 2.f));
    parent1.setLocalOrientation(quat(vec3(0.707f, 0.f, 3.14f)));
    auto child1 = transform();
    child1.setLocalPosition(vec3(0.f, 0.f, 20.f));
    child1.setLocalSize(vec3(3.f, 3.f, 3.f));
    child1.setLocalOrientation(quat(vec3(1.58f, 0.f, 0.f)));

    child1.setParent(&parent1);

    auto parent2 = transform();
    parent2.setLocalPosition(vec3(20.f, 0.f, 2.f));
    parent2.setLocalSize(vec3(2.f, 2.f, 2.f));
    parent2.setLocalOrientation(quat(vec3(0.707f, 0.f, 3.14f)));
    auto child2 = transform();
    child2.setLocalPosition(vec3(0.f, 0.f, 20.f));
    child2.setLocalSize(vec3(3.f, 3.f, 3.f));
    child2.setLocalOrientation(quat(vec3(1.58f, 0.f, 0.f)));

    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, equals_twoDisplacedTransformsWithDifferentDisplacedParents_returnsFalse)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalPosition(vec3(20.f, 0.f, 2.f));
    parent1.setLocalSize(vec3(2.f, 2.f, 2.f));
    parent1.setLocalOrientation(quat(vec3(0.707f, 0.f, 3.14f)));
    auto child1 = transform();
    child1.setLocalPosition(vec3(0.f, 0.f, 20.f));
    child1.setLocalSize(vec3(3.f, 3.f, 3.f));
    child1.setLocalOrientation(quat(vec3(1.58f, 0.f, 0.f)));

    child1.setParent(&parent1);

    auto parent2 = transform();
    parent2.setLocalPosition(vec3(2.f, 0.f, 2.f));
    parent2.setLocalSize(vec3(3.f, 3.f, 3.f));
    parent2.setLocalOrientation(quat(vec3(0.f, 0.f, 3.14f)));
    auto child2 = transform();
    child2.setLocalPosition(vec3(0.f, 0.f, 20.f));
    child2.setLocalSize(vec3(3.f, 3.f, 3.f));
    child2.setLocalOrientation(quat(vec3(1.58f, 0.f, 0.f)));

    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, equals_twoDifferentDisplacedTransformsWithDifferentDisplacedParents_returnsFalse)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalPosition(vec3(20.f, 0.f, 2.f));
    parent1.setLocalSize(vec3(2.f, 2.f, 2.f));
    parent1.setLocalOrientation(quat(vec3(0.707f, 0.f, 3.14f)));
    auto child1 = transform();
    child1.setLocalPosition(vec3(2.f, 0.f, 5.f));
    child1.setLocalSize(vec3(3.f, 3.f, 3.f));
    child1.setLocalOrientation(quat(vec3(1.58f, 0.f, 0.f)));

    child1.setParent(&parent1);

    auto parent2 = transform();
    parent2.setLocalPosition(vec3(2.f, 0.f, 2.f));
    parent2.setLocalSize(vec3(3.f, 3.f, 3.f));
    parent2.setLocalOrientation(quat(vec3(0.f, 0.f, 3.14f)));
    auto child2 = transform();
    child2.setLocalPosition(vec3(30.f, 0.f, 0.f));
    child2.setLocalSize(vec3(2.f, 2.f, 2.f));
    child2.setLocalOrientation(quat(vec3(0.707f, 0.f, 0.f)));

    child2.setParent(&parent2);

    //Act
    auto result = child1 == child2;

    //Assert
    ASSERT_FALSE(result);
}

TEST(transform, clone_originalAndCloneTransform_ShouldBeEqual)
{
    //Arrange
    auto transform1 = transform();

    auto cloned = transform1.clone();

    //Act
    auto result = transform1 == *cloned;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, clone_originalAndCloneDisplacedPositionTransform_ShouldBeDifferent)
{
    //Arrange
    auto transform1 = transform();
    auto cloned = transform1.clone();
    cloned->setLocalPosition(vec3(30.f, 0.f, 0.f));

    //Act
    auto result = transform1 != *cloned;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, clone_originalAndCloneDisplacedSizeTransform_ShouldBeDifferent)
{
    //Arrange
    auto transform1 = transform();
    auto cloned = transform1.clone();
    cloned->setLocalSize(vec3(2.f, 2.f, 2.f));

    //Act
    auto result = transform1 != *cloned;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, clone_originalAndCloneDisplacedOrientationTransform_ShouldBeDifferent)
{
    //Arrange
    auto transform1 = transform();
    auto cloned = transform1.clone();
    cloned->setLocalOrientation(quat(vec3(0.707f, 0.f, 0.f)));

    //Act
    auto result = transform1 != *cloned;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, clone_displacedOriginalAndClonedTransform_ShouldBeEqual)
{
    //Arrange
    auto transform1 = transform();
    transform1.setLocalOrientation(quat(vec3(0.707f, 0.f, 0.f)));
    auto cloned = transform1.clone();

    //Act
    auto result = transform1 == *cloned;

    //Assert
    ASSERT_TRUE(result);
}

TEST(transform, clone_displacedOriginalWithDisplacedParentAndClonedTransform_ShouldBeEqual)
{
    //Arrange
    auto parent1 = transform();
    parent1.setLocalOrientation(quat(vec3(0.707f, 0.f, 0.f)));
    auto transform1 = transform();
    transform1.setLocalPosition(vec3(12.f, 0.f, 3.5f));
    transform1.setParent(&parent1);
    auto cloned = transform1.clone();

    //Act
    auto result = transform1 == *cloned;

    //Assert
    ASSERT_TRUE(result);
}
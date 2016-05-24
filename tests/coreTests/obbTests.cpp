#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\obb.h>

using namespace phi;

class unitObbFixture :
    public testing::Test
{
public:
    obb* obb0 = nullptr;
    obb* obb1 = nullptr;
public:
    void SetUp()
    {
        auto center0 = vec3(0.0f, 0.0f, 0.0f);
        auto axisX0 = vec3(1.0f, 0.0f, 0.0f);
        auto axisY0 = vec3(0.0f, 1.0f, 0.0f);
        auto axisZ0 = vec3(0.0f, 0.0f, 1.0f);
        auto halfSizes0 = vec3(0.5f, 0.5f, 0.5f);
        obb0 = new obb(center0, axisX0, axisY0, axisZ0, halfSizes0);

        auto center1 = vec3(0.0f, 0.0f, 0.0f);
        auto axisX1 = vec3(1.0f, 0.0f, 0.0f);
        auto axisY1 = vec3(0.0f, 1.0f, 0.0f);
        auto axisZ1 = vec3(0.0f, 0.0f, 1.0f);
        auto halfSizes1 = vec3(0.5f, 0.5f, 0.5f);
        obb1 = new obb(center1, axisX1, axisY1, axisZ1, halfSizes1);
    }

    void TearDown()
    {
        safeDelete(obb0);
        safeDelete(obb1);
    }
};

TEST_F(unitObbFixture, intersects_touchingOnAxisX_notColliding)
{
    //Arrange
    obb0->center = vec3(-1.0f, 0.0f, 0.0f);
    obb1->center = vec3(1.0f, 0.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_touchingOnAxisY_notColliding)
{
    //Arrange
    obb0->center = vec3(0.0f, -1.0f, 0.0f);
    obb1->center = vec3(0.0f, 1.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_touchingOnAxisZ_notColliding)
{
    //Arrange
    obb0->center = vec3(0.0f, 0.0f, -1.0f);
    obb1->center = vec3(0.0f, 0.0f, 1.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_separatedOnAxisX_notColliding)
{
    //Arrange
    obb0->center = vec3(-2.0f, 0.0f, 0.0f);
    obb1->center = vec3(2.0f, 0.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_separatedOnAxisY_notColliding)
{
    //Arrange
    obb0->center = vec3(0.0f, -2.0f, 0.0f);
    obb1->center = vec3(0.0f, 2.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_separatedOnAxisZ_notColliding)
{
    //Arrange
    obb0->center = vec3(0.0f, 0.0f, -2.0f);
    obb1->center = vec3(0.0f, 0.0f, 2.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingOnAxisX_colliding)
{
    //Arrange
    obb1->center = vec3(0.5f, 0.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingOnAxisY_colliding)
{
    //Arrange
    obb1->center = vec3(0.0f, 0.5f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingOnAxisZ_colliding)
{
    //Arrange
    obb1->center = vec3(0.0f, 0.0f, 0.5f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb1RotatedOnAxisX_notColliding)
{
    //Arrange
    auto rotation = glm::angleAxis(PI_OVER_4, vec3(1.0f, 0.0f, 0.0f));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->center = vec3(1.0f, 0.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb1RotatedOnAxisY_colliding)
{
    //Arrange
    auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 1.0f, 0.0f));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->center = vec3(1.0f, 0.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb1RotatedOnAxisZ_colliding)
{
    //Arrange
    auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 0.0f, 1.0f));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->center = vec3(1.0f, 0.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb0RotatedOnAxisZObb1RotatedOnAxisZ_notColliding)
{
    //Arrange
    auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 0.0f, 1.0f));

    obb0->axes[0] = rotation * obb0->axes[0];
    obb0->axes[1] = rotation * obb0->axes[1];
    obb0->axes[2] = rotation * obb0->axes[2];

    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->center = vec3(0.35355339f * 2.0f, 0.35355339f * 2.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb0RotatedOnAxisZObb1RotatedOnAxisZ_colliding)
{
    //Arrange
    auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 0.0f, 1.0f));

    obb0->axes[0] = rotation * obb0->axes[0];
    obb0->axes[1] = rotation * obb0->axes[1];
    obb0->axes[2] = rotation * obb0->axes[2];

    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->center = vec3(0.34355339f * 2.0f, 0.34355339f * 2.0f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingExactly_colliding)
{
    //Arrange

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingExactlyBothRotated_colliding)
{
    //Arrange
    auto rotation = glm::angleAxis(PI_OVER_4, glm::normalize(vec3(1.0f, 1.0f, 1.0f)));

    obb0->axes[0] = rotation * obb0->axes[0];
    obb0->axes[1] = rotation * obb0->axes[1];
    obb0->axes[2] = rotation * obb0->axes[2];

    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb1EntirelyInsideObb0_colliding)
{
    //Arrange
    obb0->halfSizes = vec3(1.0f, 1.0f, 1.0f);

    auto rotation = glm::angleAxis(PI_OVER_4, glm::normalize(vec3(1.0f, 1.0f, 1.0f)));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingObb0EntirelyInsideObb1_colliding)
{
    //Arrange
    obb0->halfSizes = vec3(0.25f, 0.25f, 0.25f);

    auto rotation = glm::angleAxis(PI_OVER_4, glm::normalize(vec3(1.0f, 1.0f, 1.0f)));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingSmallerRotatedParallelepipedInsideBiggerParallelepiped_colliding)
{
    //Arrange
    obb0->halfSizes = vec3(1.0f, 0.5f, 0.5f);

    auto rotation = glm::angleAxis(PI_OVER_2, glm::normalize(vec3(0.0f, 1.0f, 0.0f)));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->halfSizes = vec3(1.0f, 0.25f, 0.5f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_TRUE(colliding);
}

TEST_F(unitObbFixture, intersects_intersectingSmallerRotatedParallelepipedNotInsideBiggerParallelepiped_notColliding)
{
    //Arrange
    obb0->halfSizes = vec3(1.0f, 0.5f, 0.5f);

    auto rotation = glm::angleAxis(PI_OVER_2, glm::normalize(vec3(0.0f, 1.0f, 0.0f)));
    obb1->axes[0] = rotation * obb1->axes[0];
    obb1->axes[1] = rotation * obb1->axes[1];
    obb1->axes[2] = rotation * obb1->axes[2];
    obb1->halfSizes = vec3(1.0f, 0.25f, 0.5f);
    obb1->center = vec3(0.0f, 0.75f, 0.0f);

    //Act
    auto colliding = obb::intersects(*obb0, *obb1);

    //Assert
    ASSERT_FALSE(colliding);
}
#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\obb.h>

using namespace phi;

namespace obbTests
{
    void assertVector(vec3 target, vec3 correct)
    {
        EXPECT_NEAR(target.x, correct.x, phi::DECIMAL_TRUNCATION);
        EXPECT_NEAR(target.y, correct.y, phi::DECIMAL_TRUNCATION);
        EXPECT_NEAR(target.z, correct.z, phi::DECIMAL_TRUNCATION);
    }

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

    TEST_F(unitObbFixture, intersects_touchingOnAxisX_colliding)
    {
        //Arrange
        obb0->center = vec3(-0.5f, 0.0f, 0.0f);
        obb1->center = vec3(0.5f, 0.0f, 0.0f);

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_TRUE(colliding);
    }

    TEST_F(unitObbFixture, intersects_touchingOnAxisY_colliding)
    {
        //Arrange
        obb0->center = vec3(0.0f, -0.5f, 0.0f);
        obb1->center = vec3(0.0f, 0.5f, 0.0f);

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_TRUE(colliding);
    }

    TEST_F(unitObbFixture, intersects_touchingOnAxisZ_colliding)
    {
        //Arrange
        obb0->center = vec3(0.0f, 0.0f, -0.5f);
        obb1->center = vec3(0.0f, 0.0f, 0.5f);

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_TRUE(colliding);
    }

    TEST_F(unitObbFixture, intersects_separatedOnAxisX_notColliding)
    {
        //Arrange
        obb0->center = vec3(-1.0f, 0.0f, 0.0f);
        obb1->center = vec3(1.0f, 0.0f, 0.0f);

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_FALSE(colliding);
    }

    TEST_F(unitObbFixture, intersects_separatedOnAxisY_notColliding)
    {
        //Arrange
        obb0->center = vec3(0.0f, -1.0f, 0.0f);
        obb1->center = vec3(0.0f, 1.0f, 0.0f);

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_FALSE(colliding);
    }

    TEST_F(unitObbFixture, intersects_separatedOnAxisZ_notColliding)
    {
        //Arrange
        obb0->center = vec3(0.0f, 0.0f, -1.0f);
        obb1->center = vec3(0.0f, 0.0f, 1.0f);

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

    TEST_F(unitObbFixture, intersects_obb1RotatedOnAxisX_colliding)
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
        ASSERT_TRUE(colliding);
    }

    TEST_F(unitObbFixture, intersects_obb1RotatedOnAxisY_colliding)
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

    TEST_F(unitObbFixture, intersects_obb1RotatedOnAxisZ_colliding)
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

    TEST_F(unitObbFixture, intersects_touchingObb0RotatedOnAxisZObb1RotatedOnAxisZ_colliding)
    {
        //Arrange
        auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 0.0f, 1.0f));

        obb0->axes[0] = rotation * obb0->axes[0];
        obb0->axes[1] = rotation * obb0->axes[1];
        obb0->axes[2] = rotation * obb0->axes[2];

        obb1->axes[0] = rotation * obb1->axes[0];
        obb1->axes[1] = rotation * obb1->axes[1];
        obb1->axes[2] = rotation * obb1->axes[2];
        obb1->center = rotation * vec3(0.0f, 0.5f, 0.0f) * 2.0f;

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_TRUE(colliding);
    }

    TEST_F(unitObbFixture, intersects_almostNotTouchingObb0RotatedOnAxisZObb1RotatedOnAxisZ_colliding)
    {
        //Arrange
        auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 0.0f, 1.0f));

        obb0->axes[0] = rotation * obb0->axes[0];
        obb0->axes[1] = rotation * obb0->axes[1];
        obb0->axes[2] = rotation * obb0->axes[2];

        obb1->axes[0] = rotation * obb1->axes[0];
        obb1->axes[1] = rotation * obb1->axes[1];
        obb1->axes[2] = rotation * obb1->axes[2];
        obb1->center = rotation * vec3(0.0f, 0.5f, 0.0f) * 1.9f;

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_TRUE(colliding);
    }

    TEST_F(unitObbFixture, intersects_almostTouchingObb0RotatedOnAxisZObb1RotatedOnAxisZ_notColliding)
    {
        //Arrange
        auto rotation = glm::angleAxis(PI_OVER_4, vec3(0.0f, 0.0f, 1.0f));

        obb0->axes[0] = rotation * obb0->axes[0];
        obb0->axes[1] = rotation * obb0->axes[1];
        obb0->axes[2] = rotation * obb0->axes[2];

        obb1->axes[0] = rotation * obb1->axes[0];
        obb1->axes[1] = rotation * obb1->axes[1];
        obb1->axes[2] = rotation * obb1->axes[2];
        obb1->center = rotation * vec3(0.0f, 0.5f, 0.0f) * 2.1f;

        //Act
        auto colliding = obb::intersects(*obb0, *obb1);

        //Assert
        ASSERT_FALSE(colliding);
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

    TEST_F(unitObbFixture, intersects_obb1EntirelyInsideObb0_colliding)
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

    TEST_F(unitObbFixture, intersects_obb0EntirelyInsideObb1_colliding)
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

    TEST_F(unitObbFixture, intersects_smallerRotatedParallelepipedInsideBiggerParallelepiped_colliding)
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

    TEST_F(unitObbFixture, intersects_smallerRotatedParallelepipedNotInsideBiggerParallelepiped_notColliding)
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

    TEST_F(unitObbFixture, getPositionAt_xAxis_correctPosition)
    {
        //Arrange

        //Act
        auto position = obb0->getPositionAt(vec3(1.0f, 0.0f, 0.0f));

        //Assert
        assertVector(position, vec3(0.5f, 0.0f, 0.0f));
    }

    TEST_F(unitObbFixture, getPositionAt_yAxisDirectionOfBiggerObb_correctPosition)
    {
        //Arrange
        obb0->halfSizes = vec3(0.5f, 1.5f, 0.25f);

        //Act
        auto position = obb0->getPositionAt(vec3(0.0f, 1.0f, 0.0f));

        //Assert
        assertVector(position, vec3(0.0f, 1.5f, 0.0f));
    }

    TEST_F(unitObbFixture, getPositionAt_zAxisDirectionOfTranslatedBiggerObb_correctPosition)
    {
        //Arrange
        obb0->halfSizes = vec3(0.5f, 1.5f, 0.25f);
        obb0->center = vec3(1.0f, 0.5f, 2.5f);

        //Act
        auto position = obb0->getPositionAt(vec3(0.0f, 0.0f, 1.0f));

        //Assert
        assertVector(position, vec3(1.0f, 0.5f, 2.75f));
    }

    TEST_F(unitObbFixture, getPositionAt_xyDirection_correctPosition)
    {
        //Arrange

        //Act
        auto position = obb0->getPositionAt(glm::normalize(vec3(1.0f, 1.0f, 0.0f)));

        //Assert
        assertVector(position, vec3(0.5f, 0.5f, 0.0f));
    }

    TEST_F(unitObbFixture, getPositionAt_negativeXYZDirection_correctPosition)
    {
        //Arrange

        //Act
        auto position = obb0->getPositionAt(glm::normalize(-vec3(1.0f)));

        //Assert
        assertVector(position, vec3(-0.5f, -0.5f, -0.5f));
    }

    TEST_F(unitObbFixture, getPositionAt_xyzDirectionOfBiggerObb_correctPosition)
    {
        //Arrange
        obb0->halfSizes = vec3(0.5f, 1.5f, 0.25f);

        //Act
        auto position = obb0->getPositionAt(glm::normalize(vec3(1.0f)));

        //Assert
        assertVector(position, vec3(0.5f, 1.5f, 0.25f));
    }
}
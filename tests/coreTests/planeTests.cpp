#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\plane.h>

using namespace phi;

namespace planeTests
{
    void assertFloat(float target, float correct)
    {
        EXPECT_NEAR(target, correct, phi::DECIMAL_TRUNCATION);
    }

    void assertVector(vec3 target, vec3 correct)
    {
        EXPECT_NEAR(target.x, correct.x, phi::DECIMAL_TRUNCATION);
        EXPECT_NEAR(target.y, correct.y, phi::DECIMAL_TRUNCATION);
        EXPECT_NEAR(target.z, correct.z, phi::DECIMAL_TRUNCATION);
    }

    TEST(plane, projectPoint_pointAboveXZAxisPlane_correctProjectedPoint)
    {
        //Arrange
        auto origin = vec3(-0.5f, 0.0f, -0.5f);
        auto normal = vec3(0.0f, 1.0f, 0.0f);
        auto plane = phi::plane(origin, normal);
        auto point = vec3(0.0f, 1.0f, 0.0f);

        //Act
        auto projection = plane.projectPoint(point);

        //Assert
        assertVector(projection, vec3());
    }

    TEST(plane, projectPoint_pointBelowXZAxisPlane_correctProjectedPoint)
    {
        //Arrange
        auto origin = vec3(-0.5f, 0.0f, -0.5f);
        auto normal = vec3(0.0f, 1.0f, 0.0f);
        auto plane = phi::plane(origin, normal);
        auto point = vec3(0.0f, -2.0f, 0.0f);

        //Act
        auto projection = plane.projectPoint(point);

        //Assert
        assertVector(projection, vec3());
    }

    TEST(plane, projectPoint_pointInFrontOfXYAxisPlaneRotatedOnY_correctProjectedPoint)
    {
        //Arrange
        auto cos45 = 0.70710678118654752440084436210485f;
        auto origin = vec3(-0.5f, 0.0f, -0.5f);
        auto normal = vec3(cos45, 0.0f, cos45);
        auto plane = phi::plane(origin, normal);
        auto point = vec3(1.0f, 1.0f, 1.0f);

        //Act
        auto projection = plane.projectPoint(point);

        //Assert
        assertVector(projection, vec3(-0.5f, 1.0f, -0.5f));
    }

    TEST(plane, projectPoint_pointExactlyOnPlaneYZ_correctProjectedPoint)
    {
        //Arrange
        auto origin = vec3(5.0f, 0.0f, -0.5f);
        auto normal = vec3(1.0f, 0.0f, 0.0f);
        auto plane = phi::plane(origin, normal);
        auto point = vec3(5.0f, 1234.4321f, 912.1994f); // Reference that no one will find out ;););)

        //Act
        auto projection = plane.projectPoint(point);

        //Assert
        assertVector(projection, vec3(5.0f, 1234.4321f, 912.1994f));
    }

    TEST(plane, projectPoint_pointBelowXZAxisPlaneRotatedOnZ_correctProjectedPoint)
    {
        //Arrange
        auto cos45 = 0.70710678118654752440084436210485f;
        auto origin = vec3(-0.5f, 0.0f, 0.0f);
        auto normal = vec3(-cos45, cos45, 0.0f);
        auto plane = phi::plane(origin, normal);
        auto point = vec3(-0.5f, 1.0f, 0.3f);

        //Act
        auto projection = plane.projectPoint(point);

        //Assert
        assertVector(projection, vec3(0.0f, 0.5f, 0.3f));
    }

    TEST(plane, intersectsLine_lineParallelToPlane_doesNotIntersects)
    {
        //Arrange
        auto origin = vec3(0.0f, 0.0f, 0.0f);
        auto normal = vec3(0.0f, 1.0f, 0.0f);
        auto plane = phi::plane(origin, normal);

        auto lineOrigin = vec3(0.0f, 1.0f, 0.3f);
        auto lineDirection = vec3(1.0f, 0.0f, 0.0f);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_FALSE(intersects);
    }

    TEST(plane, intersectsLine_yAxisLineOnXZPlane_intersectsAtCorrectDistance)
    {
        //Arrange
        auto origin = vec3(0.0f, 0.0f, 0.0f);
        auto normal = vec3(0.0f, 1.0f, 0.0f);
        auto plane = phi::plane(origin, normal);

        auto lineOrigin = vec3(0.0f, 1.0f, 0.0f);
        auto lineDirection = vec3(0.0f, 1.0f, 0.0f);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_TRUE(intersects);
        ASSERT_EQ(t, -1.0f);
    }

    TEST(plane, intersectsLine_xAxisLineOnYZPlane_intersectsAtCorrectDistance)
    {
        //Arrange
        auto origin = vec3(1.0f, 2.0f, 3.0f);
        auto normal = vec3(-1.0f, 0.0f, 0.0f);
        auto plane = phi::plane(origin, normal);

        auto lineOrigin = vec3(-3.0f, 1.0f, -7.0f);
        auto lineDirection = vec3(1.0f, 0.0f, 0.0f);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_TRUE(intersects);
        ASSERT_EQ(t, 4.0f);
    }

    TEST(plane, intersectsLine_zAxisLineOnXYPlane_intersectsAtCorrectDistance)
    {
        //Arrange
        auto origin = vec3(0.0f, 0.0f, -1.5f);
        auto normal = vec3(0.0f, 0.0f, 1.0f);
        auto plane = phi::plane(origin, normal);

        auto lineOrigin = vec3(5.0f, 5.0f, 5.0f);
        auto lineDirection = vec3(0.0f, 0.0f, 1.0f);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_TRUE(intersects);
        ASSERT_EQ(t, -6.5f);
    }

    TEST(plane, intersectsLine_diagonalLineOnXZPlane_intersectsAtCorrectDistance)
    {
        //Arrange
        auto origin = vec3(0.0f, 0.0f, 0.0f);
        auto normal = vec3(0.0f, 1.0f, 0.0f);
        auto plane = phi::plane(origin, normal);

        auto cos45 = 0.70710678118654752440084436210485f;
        auto lineOrigin = vec3(0.0f, 1.0f, 0.0f);
        auto lineDirection = vec3(cos45, -cos45, 0.0f);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_TRUE(intersects);
        ASSERT_EQ(t, 2.0f * cos45);
    }

    TEST(plane, intersectsLine_xAxisLineOnDiagonalPlane_intersectsAtCorrectDistance)
    {
        //Arrange
        auto cos45 = 0.70710678118654752440084436210485f;
        auto origin = vec3(0.0f, 0.0f, 0.0f);
        auto normal = vec3(-cos45, cos45, 0.0f);
        auto plane = phi::plane(origin, normal);

        auto lineOrigin = vec3(-1.0f, 1.0f, 0.0f);
        auto lineDirection = vec3(1.0f, 0.0f, 0.0f);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_TRUE(intersects);
        ASSERT_EQ(t, 2.0f);
    }

    TEST(plane, intersectsLine_diagonalLineOnDiagonalPlane_intersectsAtCorrectDistance)
    {
        //Arrange
        auto cos45 = 0.70710678118654752440084436210485f;
        auto origin = vec3(0.0f, 0.0f, 0.0f);
        auto normal = vec3(0.0f, -cos45, cos45);
        auto plane = phi::plane(origin, normal);

        auto lineOrigin = vec3(5.0f, -3.0f, 3.0f);
        auto lineDirection = vec3(0.0f, cos45, -cos45);

        //Act
        float t;
        auto intersects = plane.intersectsLine(lineOrigin, lineDirection, t);

        //Assert
        ASSERT_TRUE(intersects);
        assertFloat(t, 3.0f * 2.0f * cos45);
    }
}
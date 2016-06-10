#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\plane.h>

using namespace phi;

void assertVector2(vec3 target, vec3 correct)
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
    assertVector2(projection, vec3());
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
    assertVector2(projection, vec3());
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
    assertVector2(projection, vec3(-0.5f, 1.0f, -0.5f));
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
    assertVector2(projection, vec3(5.0f, 1234.4321f, 912.1994f));
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
    assertVector2(projection, vec3(0.0f, 0.5f, 0.3f));
}
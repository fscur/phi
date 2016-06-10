#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\finitePlane.h>

using namespace phi;

void assertVector(vec3 target, vec3 correct)
{
    EXPECT_NEAR(target.x, correct.x, phi::DECIMAL_TRUNCATION);
    EXPECT_NEAR(target.y, correct.y, phi::DECIMAL_TRUNCATION);
    EXPECT_NEAR(target.z, correct.z, phi::DECIMAL_TRUNCATION);
}

TEST(finitePlane, construct_planeOnPositiveXPositiveZPositiveNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(0.0f, 0.0f, 0.0f);
    auto rightPoint = vec3(0.0f, 0.0f, 1.0f);
    auto upPoint = vec3(1.0f, 0.0f, 0.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, 1.0f, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, 1.0f));
    assertVector(yAxis, vec3(1.0f, 0.0f, 0.0f));
}

TEST(finitePlane, construct_planeOnPositiveXPositiveZInvertedPositiveNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(1.0f, 0.0f, 1.0f);
    auto rightPoint = vec3(1.0f, 0.0f, 0.0f);
    auto upPoint = vec3(0.0f, 0.0f, 1.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, 1.0f, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, -1.0f));
    assertVector(yAxis, vec3(-1.0f, 0.0f, 0.0f));
}

TEST(finitePlane, construct_planeOnPositiveXPositiveZNegativeNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(0.0f, 0.0f, 0.0f);
    auto rightPoint = vec3(1.0f, 0.0f, 0.0f);
    auto upPoint = vec3(0.0f, 0.0f, 1.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, -1.0f, 0.0f));
    assertVector(xAxis, vec3(1.0f, 0.0f, 0.0f));
    assertVector(yAxis, vec3(0.0f, 0.0f, 1.0f));
}

TEST(finitePlane, construct_planeOnPositiveXNegativeZPositiveNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(1.0f, 1.0f, -1.0f);
    auto rightPoint = vec3(3.0f, 1.0f, -1.0f);
    auto upPoint = vec3(1.0f, 1.0f, -3.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, 1.0f, 0.0f));
    assertVector(xAxis, vec3(1.0f, 0.0f, 0.0f));
    assertVector(yAxis, vec3(0.0f, 0.0f, -1.0f));
}

TEST(finitePlane, construct_planeOnPositiveXNegativeZNegativeNormal_correctNormalAndAxes)
{
    //Arrange
    auto min = vec3(1.0f, -1.0f, 1.0f);
    auto max = vec3(3.0f, -3.0f, 1.0f);

    auto origin = vec3(1.0f, 1.0f, -1.0f);
    auto rightPoint = vec3(1.0f, 1.0f, -3.0f);
    auto upPoint = vec3(3.0f, 1.0f, -1.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, -1.0f, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, -1.0f));
    assertVector(yAxis, vec3(1.0f, 0.0f, 0.0f));
}

TEST(finitePlane, construct_planeOnNegativeXPositiveZPositiveNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(-1.0f, 1.0f, 1.0f);
    auto rightPoint = vec3(-3.0f, 1.0f, 1.0f);
    auto upPoint = vec3(-1.0f, 1.0f, 3.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, 1.0f, 0.0f));
    assertVector(xAxis, vec3(-1.0f, 0.0f, 0.0f));
    assertVector(yAxis, vec3(0.0f, 0.0f, 1.0f));
}

TEST(finitePlane, construct_planeOnNegativeXPositiveZNegativeNormal_correctNormalAndAxes)
{
    //Arrange
    auto min = vec3(-1.0f, 1.0f, 1.0f);
    auto max = vec3(-3.0f, 5.0f, 1.0f);

    auto origin = vec3(-1.0f, 1.0f, 1.0f);
    auto rightPoint = vec3(-1.0f, 1.0f, 3.0f);
    auto upPoint = vec3(-3.0f, 1.0f, 1.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, -1.0f, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, 1.0f));
    assertVector(yAxis, vec3(-1.0f, 0.0f, 0.0f));
}

TEST(finitePlane, construct_planeOnNegativeXNegativeZPositiveNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(-1.0f, -2.0f, -2.0f);
    auto rightPoint = vec3(-1.0f, -2.0f, -3.0f);
    auto upPoint = vec3(-3.0f, -2.0f, -2.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, 1.0f, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, -1.0f));
    assertVector(yAxis, vec3(-1.0f, 0.0f, 0.0f));
}

TEST(finitePlane, construct_planeOnNegativeXNegativeZNegativeNormal_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(-1.0f, -2.0f, -2.0f);
    auto rightPoint = vec3(-3.0f, -2.0f, -2.0f);
    auto upPoint = vec3(-1.0f, -2.0f, -3.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    assertVector(normal, vec3(0.0f, -1.0f, 0.0f));
    assertVector(xAxis, vec3(-1.0f, 0.0f, 0.0f));
    assertVector(yAxis, vec3(0.0f, 0.0f, -1.0f));
}

TEST(finitePlane, construct_plane45DegreesRampDescendingToPositiveX_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(0.0f, 5.0f, 0.0f);
    auto rightPoint = vec3(0.0f, 5.0f, 7.0f);
    auto upPoint = vec3(5.0f, 0.0f, 0.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    auto cos45 = 0.70710678118654752440084436210485f;
    assertVector(normal, vec3(cos45, cos45, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, 1.0f));
    assertVector(yAxis, vec3(cos45, -cos45, 0.0f));
}

TEST(finitePlane, construct_plane45DegreesRampDescendingToNegativeX_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(0.0f, 0.5f, 0.0f);
    auto rightPoint = vec3(0.0f, 0.5f, -0.5f);
    auto upPoint = vec3(-1.0f, -0.5f, 0.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    auto cos45 = 0.70710678118654752440084436210485f;
    assertVector(normal, vec3(-cos45, cos45, 0.0f));
    assertVector(xAxis, vec3(0.0f, 0.0f, -1.0f));
    assertVector(yAxis, vec3(-cos45, -cos45, 0.0f));
}

TEST(finitePlane, construct_plane45DegreesRampDescendingToPositiveXZ_correctNormalAndAxes)
{
    //Arrange
    auto origin = vec3(-3.0f, 6.0f, -3.0f);
    auto rightPoint = vec3(-3.0f, 0.0f, 3.0f);
    auto upPoint = vec3(3.0f, 0.0f, -3.0f);

    //Act
    auto plane = finitePlane(origin, rightPoint, upPoint);

    auto diffX = rightPoint - origin;
    auto diffY = upPoint - origin;
    auto c = glm::cross(diffX, diffY);
    auto max = origin + diffX + diffY;

    //Assert
    auto normal = plane.getNormal();
    auto xAxis = plane.getXAxis();
    auto yAxis = plane.getYAxis();
    auto cos45 = 0.70710678118654752440084436210485f;
    assertVector(normal, vec3(0.57735026918962576450914878050195f));
    assertVector(xAxis, vec3(0.0f, -cos45, cos45));
    assertVector(yAxis, vec3(cos45, -cos45, 0.0f));
}
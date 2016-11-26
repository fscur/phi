#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\ray.h>

using namespace phi;

TEST(ray, intersectsPlane_rayWithNegativeYAxisDirectionOnXZAxisPlane_collidingAtCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto normal = vec3(0.0f, 1.0f, 0.0f);
    auto plane = phi::plane(origin, normal);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_TRUE(colliding);
    ASSERT_FLOAT_EQ(t, 1.0f);
}

TEST(ray, intersectsPlane_rayWithPositiveYAxisDirectionOnXZAxisPlane_collidingAtCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto normal = vec3(0.0f, -1.0f, 0.0f);
    auto plane = phi::plane(origin, normal);
    auto r = ray(vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_TRUE(colliding);
    ASSERT_FLOAT_EQ(t, 1.0f);
}

TEST(ray, intersectsPlane_rayWithPositiveXAxisDirectionOnXZAxisPlane_notCollidingDistanceNotSet)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto normal = vec3(0.0f, 1.0f, 0.0f);
    auto plane = phi::plane(origin, normal);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST(ray, intersectsPlane_rayWithPositiveYAxisDirectionOnXYAxisPlane_notCollidingDistanceNotSet)
{
    //Arrange
    auto origin = vec3(0.0f, 0.0f, -0.5f);
    auto normal = vec3(0.0f, 0.0f, -1.0f);
    auto plane = phi::plane(origin, normal);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST(ray, intersectsPlane_rayWithNegativeYAxisDirectionOnRampPlane_collidingAtCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, 0.0f);
    auto normal = vec3(-0.70710678118654752440084436210485f, 0.70710678118654752440084436210485f, 0.0f);
    auto plane = phi::plane(origin, normal);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_TRUE(colliding);
    ASSERT_FLOAT_EQ(t, 0.5f);
}

TEST(ray, intersectsFinitePlane_rayWithNegativeYAxisDirectionOnXZAxisPlane_collidingAtCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto rightPoint = vec3(0.5f, 0.0f, -0.5f);
    auto upPoint = vec3(-0.5f, 0.0f, 0.5f);
    auto plane = orientedPlane(origin, rightPoint, upPoint);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_TRUE(colliding);
    ASSERT_FLOAT_EQ(t, 1.0f);
}

TEST(ray, intersectsFinitePlane_rayWithPositiveYAxisDirectionOnXZAxisPlane_collidingAtCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto rightPoint = vec3(0.5f, 0.0f, -0.5f);
    auto upPoint = vec3(-0.5f, 0.0f, 0.5f);
    auto plane = orientedPlane(origin, rightPoint, upPoint);
    auto r = ray(vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_TRUE(colliding);
    ASSERT_FLOAT_EQ(t, 1.0f);
}

TEST(ray, intersectsFinitePlane_rayWithPositiveXAxisDirectionOnXZAxisPlane_notCollidingDistanceNotSet)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto rightPoint = vec3(0.5f, 0.0f, -0.5f);
    auto upPoint = vec3(-0.5f, 0.0f, 0.5f);
    auto plane = orientedPlane(origin, rightPoint, upPoint);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_FALSE(colliding);
}

TEST(ray, intersectsFinitePlane_rayWithNegativeYAxisDirectionOnXZAxisPlane_notCollidingButCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, -0.5f);
    auto rightPoint = vec3(0.5f, 0.0f, -0.5f);
    auto upPoint = vec3(-0.5f, 0.0f, 0.5f);
    auto plane = orientedPlane(origin, rightPoint, upPoint);
    auto r = ray(vec3(1.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_FALSE(colliding);
    ASSERT_FLOAT_EQ(t, 1.0f);
}

TEST(ray, intersectsFinitePlane_rayWithNegativeYAxisDirectionOnRampPlane_collidingAtCorrectDistance)
{
    //Arrange
    auto origin = vec3(-0.5f, 0.0f, 0.0f);
    auto rightPoint = vec3(-0.5f, 0.0f, 1.0f);
    auto upPoint = vec3(0.5f, 1.0f, 0.0f);
    auto plane = orientedPlane(origin, rightPoint, upPoint);
    auto r = ray(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, -1.0f, 0.0f));
    float t;

    //Act
    auto colliding = r.intersects(plane, t);

    //Assert
    ASSERT_TRUE(colliding);
    ASSERT_FLOAT_EQ(t, 0.5f);
}
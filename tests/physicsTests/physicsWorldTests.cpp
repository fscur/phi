#include <precompiled.h>
#include <phi.h>

#include <gtest\gtest.h>

#include <core\node.h>

#include <physics\physicsWorld.h>

using namespace phi;

class physicsWorldWithCubeFixture :
    public testing::Test
{
public:
    physicsWorld* physicsWorld = nullptr;
    node* cubeNode = nullptr;

public:
    void SetUp()
    {
        cubeNode = new node();
        auto cubeCollider = new boxCollider(vec3(), vec3(1.0f));
        cubeNode->addComponent(cubeCollider);

        physicsWorld = new phi::physicsWorld();
        physicsWorld->addCollider(cubeCollider);
    }

    void TearDown()
    {
        safeDelete(physicsWorld);
        safeDelete(cubeNode);
    }
};

TEST_F(physicsWorldWithCubeFixture, sweep_cubeAlmostCollidingFromTheSide_doesNotCollide)
{
    //Arrange
    auto cube = new boxCollider(vec3(1.001f, 1.0f, 0.0f), vec3(1.0f));
    //auto cube = new boxCollider(vec3(1.00010002f, 1.00079584f, 0.0f), vec3(1.0f));

    auto sweepTest = sweep::singleToSceneTest();
    sweepTest.collider = cube;
    sweepTest.transform = new transform();
    sweepTest.parameters.direction = vec3(0.0f, -1.0f, 0.0f);
    sweepTest.parameters.distance = 1.0f;
    sweepTest.parameters.maximumHits = 1;
    sweepTest.parameters.disregardDivergentNormals = false;

    //Act
    auto testResult = physicsWorld->sweep(sweepTest);

    //Assert
    ASSERT_FALSE(testResult.collided);
}

TEST_F(physicsWorldWithCubeFixture, sweep_cubeHittingFromTheSide_collidesWithCorrectDistance)
{
    //Arrange
    auto cube = new boxCollider(vec3(1.5f, 0.0f, 0.0f), vec3(1.0f));

    auto sweepTest = sweep::singleToSceneTest();
    sweepTest.collider = cube;
    sweepTest.transform = new transform();
    sweepTest.parameters.direction = vec3(-1.0f, 0.0f, 0.0f);
    sweepTest.parameters.distance = 0.5f;
    sweepTest.parameters.maximumHits = 1;
    sweepTest.parameters.disregardDivergentNormals = false;

    //Act
    auto testResult = physicsWorld->sweep(sweepTest);

    //Assert
    ASSERT_TRUE(testResult.collided);
    ASSERT_EQ(testResult.collisions[0].distance, 0.5f);
}
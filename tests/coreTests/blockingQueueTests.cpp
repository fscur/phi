#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\blockingQueue.h>

TEST(blockingQueue, pushRef)
{
    //arrange
    phi::blockingQueue<std::string> queue;
    std::string member("first member");

    //act
    queue.push(member);

    //assert
    ASSERT_EQ(queue.size(), 1u);
}

TEST(blockingQueue, pushValue)
{
    //arrange
    phi::blockingQueue<int> queue;

    //act
    queue.push(0);

    //assert
    ASSERT_EQ(queue.size(), 1u);
}

TEST(blockingQueue, notEmpty)
{
    //arrange
    phi::blockingQueue<int> queue;
    queue.push(0);

    //act & assert
    ASSERT_FALSE(queue.empty());
}

TEST(blockingQueue, empty)
{
    //arrange
    phi::blockingQueue<int> queue;

    //act & assert
    ASSERT_TRUE(queue.empty());
}

TEST(blockingQueue, size)
{
    //arrange
    phi::blockingQueue<int> queue;
    queue.push(0);
    queue.push(1);
    queue.push(2);

    //act & assert
    ASSERT_EQ(queue.size(), 3u);
}

TEST(blockingQueue, back)
{
    //arrange
    phi::blockingQueue<int> queue;
    queue.push(0);
    queue.push(1);
    queue.push(2);

    //act
    auto backMember = queue.back();

    //assert
    ASSERT_EQ(backMember, 2);
}

TEST(blockingQueue, emplace)
{
    //arrange
    phi::blockingQueue<int> queue;
    queue.push(1);
    queue.push(2);

    //act
    queue.emplace(0);

    //assert
    ASSERT_EQ(queue.size(), 3u);
    ASSERT_EQ(queue.back(), 0);
}

TEST(blockingQueue, front)
{
    //arrange
    phi::blockingQueue<int> queue;
    queue.push(0);
    queue.push(1);
    queue.push(2);

    //act
    auto frontMember = queue.front();

    //assert
    ASSERT_EQ(frontMember, 0);
    ASSERT_EQ(queue.size(), 3u);
}

TEST(blockingQueue, pop)
{
    //arrange
    phi::blockingQueue<int> queue;
    queue.push(0);
    queue.push(1);
    queue.push(2);

    //act
    queue.pop();
    auto frontMember = queue.front();

    //assert
    ASSERT_EQ(frontMember, 1);
    ASSERT_EQ(queue.size(), 2u);
}

TEST(blockingQueue, producerConsumer)
{
    //arrange & act
    phi::blockingQueue<int> queue;

    std::thread consumer([&]() 
    {
        for (int i = 0; i < 10; ++i)
        {
            queue.pop();
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::thread producer([&]()
    {
        for (int i = 0; i < 10; ++i)
        {
            queue.push(i);
        }
    });

    producer.join();
    consumer.join();

    //assert
    ASSERT_EQ(queue.size(), 0u);
}
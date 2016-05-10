#include <precompiled.h>
#include <gtest\gtest.h>
#include <fakeit\fakeit.hpp>

#include <core\node.h>

using namespace fakeit;
using namespace phi;

struct SomeInterface {
    virtual int foo(int) = 0;
    virtual int bar(std::string) = 0;
};

TEST(node, optimize)
{
    auto node = new phi::node();
    auto child1 = new phi::node();
    auto child2 = new phi::node();
    auto child3 = new phi::node();

    node->addChild(child1);
    node->addChild(child2);
    node->addChild(child3);

    auto expected = 3u;
    auto result = node->getChildren()->size();
    ASSERT_EQ(expected, result);

    Mock<SomeInterface> mock;
    When(Method(mock, foo)).Return(2);
    SomeInterface& i = mock.get();
    ASSERT_EQ(i.foo(3), 2);

    safeDelete(node);
}
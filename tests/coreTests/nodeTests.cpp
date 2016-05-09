#include <precompiled.h>
#include <gtest\gtest.h>
#include <core\node.h>

TEST(node, optimize)
{
    auto node = phi::node();
    auto child1 = new phi::node();
    auto child2 = new phi::node();
    auto child3 = new phi::node();

    node.addChild(child1);
    node.addChild(child2);
    node.addChild(child3);

    auto expectedChildren = 3u;
    ASSERT_EQ(node.getChildren()->size(), expectedChildren);
}
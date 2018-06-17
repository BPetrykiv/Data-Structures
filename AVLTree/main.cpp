#include <gtest/gtest.h>
#include "avltree.hpp"
#include <vector>


class TestAVLTree : public ::testing::Test
{
public:
    AVLTree<int>* tree;

    TestAVLTree()
    {
        tree = new AVLTree<int>();
    }
    ~TestAVLTree()
    {
        delete tree;
    }
};

TEST(AVLTreeTest, ConstructibleTreeTest)
{
    AVLTree<int> tree;

    EXPECT_EQ(tree.size(), 0);
    ASSERT_TRUE(tree.isEmpty());

    std::vector<int> empty = std::move(tree.toContainer<std::vector<int>>());
    ASSERT_TRUE(empty.empty());
}

TEST(AVLTreeTest, CopyContructibleTreeTest)
{
    AVLTree<int> origin = {1,2,3,4};
    AVLTree<int> copy(origin);

    ASSERT_EQ(origin.size(), copy.size());

    auto v1 = origin.toContainer<std::vector<int>>();
    auto v2 = copy.toContainer<std::vector<int>>();

    ASSERT_EQ(v1, v2);
}

TEST(AVLTreeTest, CopyAssignmentTreeTest)
{
    AVLTree<int> origin = {1,2,3,4};
    AVLTree<int> copy;
    copy = origin;

    ASSERT_EQ(origin.size(), copy.size());

    auto v1 = origin.toContainer<std::vector<int>>();
    auto v2 = copy.toContainer<std::vector<int>>();

    ASSERT_EQ(v1, v2);
}

TEST(AVLTreeTest, MoveTreeTest)
{
    AVLTree<int> orig = {1,2,8,74,5};
    auto moved = std::move(orig);

    ASSERT_TRUE(orig.isEmpty());
    ASSERT_TRUE(!moved.isEmpty());

    ASSERT_EQ(moved.size(), 5);

    auto vec = moved.toContainer<std::vector<int>>();
    ASSERT_EQ(vec.size(), 5);
}

TEST_F(TestAVLTree, InsertingAVLTreeTest)
{
    tree->insert(5);
    ASSERT_TRUE(tree->find(5));
    ASSERT_EQ(tree->size(), 1);

    tree->insert(8);
    tree->insert(4);
    tree->insert(6);
    tree->insert(5);

    ASSERT_EQ(tree->size(), 5);

    std::vector<int> v{4,5,5,6,8};
    ASSERT_EQ(v, tree->toContainer<std::vector<int>>());

}

TEST_F(TestAVLTree, RemovingAVLTreeTest)
{
    tree->insert(8);
    tree->insert(4);
    tree->insert(6);
    tree->insert(5);

    tree->remove(5);
    ASSERT_EQ(tree->size(), 3);
    ASSERT_FALSE(tree->find(5));

    tree->remove(4);
    tree->remove(6);
    tree->remove(8);

    ASSERT_TRUE(tree->isEmpty());

}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}



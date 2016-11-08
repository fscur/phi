#include <precompiled.h>
#include <gtest\gtest.h>

#include <core\node.h>
#include <core\model.h>
#include <context\layer.h>
#include <layers\nodeSelection\selectionInputController.h>

using namespace phi;

namespace selectionInputControllerTests
{
    class selectionInputControllerFixture :
        public testing::Test
    {
    public:
        selectionLayerBehaviour* _selectionBehaviour = nullptr;
        selectionInputController* _selectionController = nullptr;
    public:
        void SetUp()
        {
            _selectionBehaviour = new selectionLayerBehaviour();
            _selectionController = new selectionInputController(new commandsManager(), _selectionBehaviour);
        }

        void AssignNodeSelectionChangedToBehaviour(node* node)
        {
            node->selectionChanged.assign(std::bind(&selectionLayerBehaviour::onNodeSelectionChanged, _selectionBehaviour, std::placeholders::_1));
        }

        void TearDown()
        {
            safeDelete(_selectionBehaviour);
            safeDelete(_selectionController);
        }
    };

    node* newModelNode(string name)
    {
        auto newNode = new node(name);
        newNode->addComponent(new model());
        return newNode;
    }

    TEST_F(selectionInputControllerFixture, select_unselectedNode_selectTheNode)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto node = newModelNode("node");
        auto eventRaised = false;
        sceneRoot->addChild(node);

        AssignNodeSelectionChangedToBehaviour(node);

        node->selectionChanged.assign([&](phi::node* node) { eventRaised = true; });

        //Act
        _selectionController->select(node);

        //Assert
        ASSERT_TRUE(node->isSelected());
        ASSERT_TRUE(eventRaised);

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, deselectAll_selectedNode_deselectsNode)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto node = newModelNode("node");
        sceneRoot->addChild(node);

        AssignNodeSelectionChangedToBehaviour(node);

        _selectionController->select(node);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        node->selectionChanged.assign([&](phi::node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(node->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_nodeToSelectHasChild_selectsTheParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(parent);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, deselectAll_nodeToSelectHasChild_leavesParentAndChildUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = new node("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(child);
        AssignNodeSelectionChangedToBehaviour(parent);

        _selectionController->select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_nodeToSelectHasParent_selectsTheParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, deselectAll_nodeToSelectHasParent_leavesParentAndChildUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_nodeToSelectHasSelectedParent_selectsChildAndUnselectParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_TRUE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, deselectAll_nodeToSelectHasSelectedParent_leavesParentAndChildUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(parent);
        _selectionController->select(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_nodeToSelectHasUnselectedGrandparent_selectsGrandparent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = new node("parent");
        auto child = new node("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        AssignNodeSelectionChangedToBehaviour(grandparent);
        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(grandparent->isSelected());
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, deselectAll_selectedGrandparent_leavesAllUnselected)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = new node("parent");
        auto child = new node("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        AssignNodeSelectionChangedToBehaviour(grandparent);
        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(child);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(grandparent->isSelected());
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_nodeToSelectHasSelectedGrandparent_selectsParentAndUnselectGrandparent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        AssignNodeSelectionChangedToBehaviour(grandparent);
        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(grandparent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(grandparent->isSelected());
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_nodeToSelectHasUnselectedGrandparentAndSelectedParent_selectsChildAndUnselectsParent)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto grandparent = newModelNode("grandparent");
        auto parent = newModelNode("parent");
        auto child = newModelNode("child");

        parent->addChild(child);
        grandparent->addChild(parent);
        sceneRoot->addChild(grandparent);

        AssignNodeSelectionChangedToBehaviour(grandparent);
        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(grandparent);
        _selectionController->select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        grandparent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(grandparent->isSelected());
        ASSERT_FALSE(parent->isSelected());
        ASSERT_TRUE(child->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, execute_nodeToSelectHasSelectedBrother_selectesNode)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child1 = newModelNode("child1");
        auto child2 = newModelNode("child2");

        parent->addChild(child1);
        parent->addChild(child2);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child1);
        AssignNodeSelectionChangedToBehaviour(child2);

        _selectionController->select(parent);
        _selectionController->select(child1);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child2);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_TRUE(child2->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, executeUndo_nodeToSelectHasSelectedBrother_leavesTreeUntouched)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto child1 = newModelNode("child1");
        auto child2 = newModelNode("child2");

        parent->addChild(child1);
        parent->addChild(child2);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child1);
        AssignNodeSelectionChangedToBehaviour(child2);

        _selectionController->select(parent);
        _selectionController->select(child1);
        _selectionController->select(child2);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_FALSE(child2->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, select_groupOfNodeToSelectHasBrotherGroupContainingOneSelectedNode_selectsBrotherGroup)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto group1 = new node("group1");
        auto child1 = new node("child1");
        auto child2 = newModelNode("child2");
        auto group2 = newModelNode("group2");
        auto child3 = new node("child3");
        auto child4 = new node("child4");

        group1->addChild(child1);
        group1->addChild(child2);

        group2->addChild(child3);
        group2->addChild(child4);

        parent->addChild(group1);
        parent->addChild(group2);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(group1);
        AssignNodeSelectionChangedToBehaviour(group2);
        AssignNodeSelectionChangedToBehaviour(child1);
        AssignNodeSelectionChangedToBehaviour(child2);
        AssignNodeSelectionChangedToBehaviour(child3);
        AssignNodeSelectionChangedToBehaviour(child4);

        _selectionController->select(parent);
        _selectionController->select(group1);
        _selectionController->select(child2);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 2u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child3);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(group1->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_FALSE(child2->isSelected());
        ASSERT_TRUE(group2->isSelected());
        ASSERT_FALSE(child3->isSelected());
        ASSERT_FALSE(child4->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, deselectAll_selectedGroup_deselectsAll)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = new node("parent");
        auto group1 = new node("group1");
        auto child1 = new node("child1");
        auto child2 = newModelNode("child2");
        auto group2 = newModelNode("group2");
        auto child3 = new node("child3");
        auto child4 = new node("child4");

        group1->addChild(child1);
        group1->addChild(child2);

        group2->addChild(child3);
        group2->addChild(child4);

        parent->addChild(group1);
        parent->addChild(group2);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(group1);
        AssignNodeSelectionChangedToBehaviour(child1);
        AssignNodeSelectionChangedToBehaviour(child2);
        AssignNodeSelectionChangedToBehaviour(group2);
        AssignNodeSelectionChangedToBehaviour(child3);
        AssignNodeSelectionChangedToBehaviour(child4);

        _selectionController->select(parent);
        _selectionController->select(group1);
        _selectionController->select(child2);
        _selectionController->select(child3);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 1u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        group2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child1->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child2->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child3->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child4->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->deselectAll();

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_FALSE(parent->isSelected());
        ASSERT_FALSE(group1->isSelected());
        ASSERT_FALSE(child1->isSelected());
        ASSERT_FALSE(child2->isSelected());
        ASSERT_FALSE(group2->isSelected());
        ASSERT_FALSE(child3->isSelected());
        ASSERT_FALSE(child4->isSelected());

        safeDelete(sceneRoot);
    }

    TEST_F(selectionInputControllerFixture, execute_nodeToSelectHasSelectedParentButDoesNotHaveModelComponent_leavesTreeUntouched)
    {
        //Arrange
        auto sceneRoot = new node("root");
        auto parent = newModelNode("parent");
        auto child = new node("child");

        parent->addChild(child);
        sceneRoot->addChild(parent);

        AssignNodeSelectionChangedToBehaviour(parent);
        AssignNodeSelectionChangedToBehaviour(child);

        _selectionController->select(parent);

        auto eventRaisedCount = 0u;
        auto expectedEventRaisedCount = 0u;
        parent->selectionChanged.assign([&](node* node) { eventRaisedCount++; });
        child->selectionChanged.assign([&](node* node) { eventRaisedCount++; });

        //Act
        _selectionController->select(child);

        //Assert
        ASSERT_EQ(expectedEventRaisedCount, eventRaisedCount);
        ASSERT_TRUE(parent->isSelected());
        ASSERT_FALSE(child->isSelected());

        safeDelete(sceneRoot);
    }
}
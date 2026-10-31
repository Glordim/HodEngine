#include <gtest/gtest.h>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Button.hpp>
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/CanvasManager.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>
#include <HodEngine/UI2/LayoutParamsFactory.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>

namespace hod
{
	namespace
	{
		// The view's position as is: the canvas' center is the view's origin
		bool MapAsIs(const Vector2& viewPosition, const Vector2&, Vector2& outCanvasPosition)
		{
			outCanvasPosition = viewPosition;
			return true;
		}

		// A Button node stretched over the whole canvas, with that action
		Node* AddButton(Canvas& canvas, const char* action)
		{
			AnchoredLayoutParams* layoutParams = DefaultAllocator::GetInstance().New<AnchoredLayoutParams>();
			layoutParams->SetAnchorMin(Vector2::Zero);
			layoutParams->SetAnchorMax(Vector2::One);

			Node* node = DefaultAllocator::GetInstance().New<Node>();
			node->SetName("Button");
			node->SetInteraction<Button>().SetAction(action);
			canvas.AddChild(node, layoutParams);
			return node;
		}
	}

	class CanvasManagerTest : public ::testing::Test
	{
	protected:
		static void SetUpTestSuite()
		{
			NodeFactory::CreateInstance()->Register<Node>();
			LayoutParamsFactory::CreateInstance()->Register<AnchoredLayoutParams>();
			LayoutFactory::CreateInstance();
			InteractionFactory::CreateInstance()->Register<Button>();
		}

		static void TearDownTestSuite()
		{
			InteractionFactory::DestroyInstance();
			LayoutFactory::DestroyInstance();
			LayoutParamsFactory::DestroyInstance();
			NodeFactory::DestroyInstance();
		}

		void SetUp() override
		{
			CanvasManager::CreateInstance()->SetPointerSource(CanvasManager::PointerSource::Manual);
		}

		void TearDown() override
		{
			CanvasManager::DestroyInstance();
		}

		// A frame with the pointer at position (nullptr: no pointer)
		static void Frame(const Vector2* position, bool pressed)
		{
			if (position != nullptr)
			{
				CanvasManager::GetInstance()->SetPointer(*position, Vector2(1920.0f, 1080.0f), pressed);
			}
			CanvasManager::GetInstance()->Update();
		}
	};

	TEST_F(CanvasManagerTest, HighestPriorityCanvasCatchingThePointerTakesIt)
	{
		Canvas above;
		Canvas below;
		Node*  aboveButton = AddButton(above, "Above");
		Node*  belowButton = AddButton(below, "Below");
		CanvasManager::GetInstance()->AddCanvas(&below);
		CanvasManager::GetInstance()->AddCanvas(&above);
		CanvasManager::GetInstance()->SetPointerMapping(&below, &MapAsIs, 0);
		CanvasManager::GetInstance()->SetPointerMapping(&above, &MapAsIs, 10);

		Vector2 center = Vector2::Zero;
		Frame(&center, false);
		EXPECT_EQ(above.GetHoveredNode(), aboveButton);
		EXPECT_EQ(below.GetHoveredNode(), nullptr);
		EXPECT_TRUE(CanvasManager::GetInstance()->IsPointerOverUI());

		// Nothing catching it above: it goes through
		above.RemoveChild(aboveButton);
		DefaultAllocator::GetInstance().Delete(aboveButton);
		Frame(&center, false);
		EXPECT_EQ(below.GetHoveredNode(), belowButton);

		// No pointer this frame: nothing is hovered anymore
		Frame(nullptr, false);
		EXPECT_EQ(below.GetHoveredNode(), nullptr);
		EXPECT_FALSE(CanvasManager::GetInstance()->IsPointerOverUI());

		CanvasManager::GetInstance()->RemoveCanvas(&above);
		CanvasManager::GetInstance()->RemoveCanvas(&below);
	}

	TEST_F(CanvasManagerTest, OnlyAPressStartedOverTheCanvasPressesIt)
	{
		Canvas canvas;
		AddButton(canvas, "Action");
		CanvasManager::GetInstance()->AddCanvas(&canvas);
		// Only displayed over the left half of the view
		CanvasManager::GetInstance()->SetPointerMapping(
			&canvas,
			[](const Vector2& viewPosition, const Vector2&, Vector2& outCanvasPosition)
			{
				outCanvasPosition = viewPosition;
				return viewPosition.GetX() < 0.0f;
			},
			0);

		uint32_t                  actionCount = 0;
		Canvas::ActionEvent::Slot onAction([&](const String&, Node&) { ++actionCount; });
		canvas.GetActionEvent().Connect(onAction);

		// Pressed out of the canvas, then moved over the button and released: no click
		Vector2 left(-100.0f, 0.0f);
		Vector2 right(100.0f, 0.0f);
		Frame(&right, true);
		Frame(&left, true);
		EXPECT_EQ(canvas.GetPressedNode(), nullptr);
		Frame(&left, false);
		EXPECT_EQ(actionCount, 0u);

		// Pressed and released over it: a click
		Frame(&left, true);
		EXPECT_NE(canvas.GetPressedNode(), nullptr);
		Frame(&left, false);
		EXPECT_EQ(actionCount, 1u);

		CanvasManager::GetInstance()->RemoveCanvas(&canvas);
	}

	TEST_F(CanvasManagerTest, CanvasPressedKeepsThePointerUntilReleased)
	{
		Canvas above;
		Canvas below;
		AddButton(above, "Above");
		Node* belowButton = AddButton(below, "Below");
		CanvasManager::GetInstance()->AddCanvas(&above);
		CanvasManager::GetInstance()->AddCanvas(&below);

		// above only covers the left half of the view
		CanvasManager::GetInstance()->SetPointerMapping(
			&above,
			[](const Vector2& viewPosition, const Vector2&, Vector2& outCanvasPosition)
			{
				outCanvasPosition = viewPosition;
				return viewPosition.GetX() < 0.0f;
			},
			10);
		CanvasManager::GetInstance()->SetPointerMapping(&below, &MapAsIs, 0);

		uint32_t                  actionCount = 0;
		Canvas::ActionEvent::Slot onAction([&](const String&, Node&) { ++actionCount; });
		above.GetActionEvent().Connect(onAction);

		Vector2 left(-100.0f, 0.0f);
		Vector2 right(100.0f, 0.0f);
		Frame(&left, true);
		EXPECT_NE(above.GetPressedNode(), nullptr);

		// Out of above while pressed: below doesn't get it, above loses its press (no click)
		Frame(&right, true);
		EXPECT_EQ(below.GetHoveredNode(), nullptr);
		EXPECT_EQ(above.GetPressedNode(), nullptr);
		Frame(&right, false);
		EXPECT_EQ(actionCount, 0u);
		EXPECT_EQ(below.GetHoveredNode(), belowButton);

		CanvasManager::GetInstance()->RemoveCanvas(&above);
		CanvasManager::GetInstance()->RemoveCanvas(&below);
	}

	TEST_F(CanvasManagerTest, FindByPathFollowsTheNames)
	{
		Node  root;
		Node* panel = DefaultAllocator::GetInstance().New<Node>();
		panel->SetName("Panel");
		root.AddChild(panel);
		Node* title = DefaultAllocator::GetInstance().New<Node>();
		title->SetName("Title");
		panel->AddChild(title);

		EXPECT_EQ(root.FindByPath(""), &root);
		EXPECT_EQ(root.FindByPath("Panel"), panel);
		EXPECT_EQ(root.FindByPath("Panel/Title"), title);
		EXPECT_EQ(root.FindByPath<Node>("Panel/Title"), title);
		EXPECT_EQ(root.FindByPath("Panel/Missing"), nullptr);
		EXPECT_EQ(root.FindByPath("Title"), nullptr);
	}
}

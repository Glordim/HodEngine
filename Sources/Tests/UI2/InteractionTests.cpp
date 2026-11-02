#include <gtest/gtest.h>

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/UI2/AnchoredLayoutParams.hpp>
#include <HodEngine/UI2/Button.hpp>
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/InteractionFactory.hpp>
#include <HodEngine/UI2/LayoutFactory.hpp>
#include <HodEngine/UI2/LayoutParamsFactory.hpp>
#include <HodEngine/UI2/Node.hpp>
#include <HodEngine/UI2/NodeFactory.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>
#include <HodEngine/UI2/ScrollView.hpp>
#include <HodEngine/UI2/Slider.hpp>
#include <HodEngine/UI2/Toggle.hpp>

namespace hod::interaction_tests // named: the other test files have helpers of the same names (unity builds)
{
	namespace
	{
		// A node under parent, spanning its anchors grown by sizeDelta (or of that size when anchored on a point)
		Node* AddNode(Node* parent, const char* name, const Vector2& anchorMin, const Vector2& anchorMax, const Vector2& size = Vector2::Zero,
		              const Vector2& alignment = Vector2(0.5f, 0.5f))
		{
			AnchoredLayoutParams* layoutParams = DefaultAllocator::GetInstance().New<AnchoredLayoutParams>();
			layoutParams->SetAnchorMin(anchorMin);
			layoutParams->SetAnchorMax(anchorMax);
			layoutParams->SetAlignment(alignment);
			layoutParams->SetSize(size);

			Node* node = DefaultAllocator::GetInstance().New<Node>();
			node->SetName(name);
			parent->AddChild(node, layoutParams);
			return node;
		}

		AnchoredLayoutParams* GetAnchored(Node* node)
		{
			return LayoutParams::Cast<AnchoredLayoutParams>(node->GetLayoutParams());
		}

		PropertyOverride RotationOverride(uint64_t target, float rotation)
		{
			Node scratch;
			scratch.SetRotation(rotation);
			PropertyValue value;
			value.Read(scratch, "Rotation");

			PropertyOverride propertyOverride;
			propertyOverride._target = target;
			propertyOverride._property = "Rotation";
			propertyOverride.SetValue(value);
			return propertyOverride;
		}

		// A node reference of interaction, as the editor sets it: through reflection
		template<typename _Interaction_>
		NodeRefBase& GetReference(_Interaction_& interaction, const char* name)
		{
			ReflectionPropertyObject* property = _Interaction_::GetReflectionDescriptor().template FindProperty<ReflectionPropertyObject>(name);
			return *static_cast<NodeRefBase*>(property->GetInstance(&interaction));
		}

		// Laid out, then the pointer
		void Pointer(Canvas& canvas, const Vector2& position, bool pressed, const Vector2& scroll = Vector2::Zero)
		{
			canvas.UpdateLayout();
			canvas.UpdatePointer(position, pressed, scroll);
		}

		void Click(Canvas& canvas, const Vector2& position)
		{
			Pointer(canvas, position, false);
			Pointer(canvas, position, true);
			Pointer(canvas, position, false);
		}

		// A frame of the interactions (transitions, scrolling), laid out after
		void Frame(Canvas& canvas, float deltaTime = 1.0f / 60.0f)
		{
			canvas.Update(deltaTime);
			canvas.UpdateLayout();
		}
	}

	class InteractionTest : public ::testing::Test
	{
	protected:
		static void SetUpTestSuite()
		{
			NodeFactory::CreateInstance()->Register<Node>();
			LayoutParamsFactory::CreateInstance()->Register<AnchoredLayoutParams>();
			LayoutFactory::CreateInstance();
			InteractionFactory* interactionFactory = InteractionFactory::CreateInstance();
			interactionFactory->Register<Button>();
			interactionFactory->Register<Toggle>();
			interactionFactory->Register<Slider>();
			interactionFactory->Register<ScrollView>();
		}

		static void TearDownTestSuite()
		{
			InteractionFactory::DestroyInstance();
			LayoutFactory::DestroyInstance();
			LayoutParamsFactory::DestroyInstance();
			NodeFactory::DestroyInstance();
		}

		void AssignIds()
		{
			uint64_t nextLocalId = 1;
			_canvas.GetRootNode()->AssignLocalIds(nextLocalId);
			_canvas.UpdateLayout();
		}

		Canvas _canvas; // 1920 x 1080, its origin at its center (y up)
	};

	TEST_F(InteractionTest, ToggleSwitchesOnClickWithItsOnLook)
	{
		Node*   toggleNode = AddNode(_canvas.GetRootNode(), "Toggle", Vector2::Zero, Vector2::One);
		Node*   check = AddNode(toggleNode, "Check", Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector2(10.0f, 10.0f));
		Toggle& toggle = toggleNode->SetInteraction<Toggle>();
		toggle.SetAction("Switched");
		toggle.SetTransitionDuration(0.0f);
		AssignIds();
		toggle.GetOnOverrides().PushBack(RotationOverride(check->GetLocalId(), 90.0f));
		// The state's look over the On one
		toggle.GetOverrides(Selectable::State::Pressed)->PushBack(RotationOverride(check->GetLocalId(), 45.0f));

		int  actions = 0;
		int  changes = 0;
		bool lastValue = false;
		Canvas::ActionEvent::Slot onAction([&](const String& action, Node&) { actions += (action == "Switched") ? 1 : 0; });
		_canvas.GetActionEvent().Connect(onAction);
		Toggle::ValueChangedEvent::Slot onChanged(
			[&](bool on)
			{
				++changes;
				lastValue = on;
			});
		toggle.GetValueChangedEvent().Connect(onChanged);
		Frame(_canvas);

		Pointer(_canvas, Vector2::Zero, false);
		Pointer(_canvas, Vector2::Zero, true);
		Frame(_canvas);
		EXPECT_FLOAT_EQ(check->GetRotation(), 45.0f); // pressed, still off
		Pointer(_canvas, Vector2::Zero, false);
		Frame(_canvas);
		EXPECT_TRUE(toggle.IsOn());
		EXPECT_FLOAT_EQ(check->GetRotation(), 90.0f);
		EXPECT_EQ(actions, 1);
		EXPECT_EQ(changes, 1);
		EXPECT_TRUE(lastValue);

		Click(_canvas, Vector2::Zero);
		Frame(_canvas);
		EXPECT_FALSE(toggle.IsOn());
		EXPECT_FLOAT_EQ(check->GetRotation(), 0.0f);
		EXPECT_EQ(changes, 2);

		// From code: the event, not the action
		toggle.SetOn(true);
		Frame(_canvas);
		EXPECT_FLOAT_EQ(check->GetRotation(), 90.0f);
		EXPECT_EQ(actions, 2);
		EXPECT_EQ(changes, 3);

		// Saved on, loaded on: its On look applied from the start
		Document document;
		ASSERT_TRUE(toggleNode->SerializeAsChildInDocument(document.GetRootNode()));
		Canvas other;
		Node*  copy = other.GetRootNode()->AddChildFromDocument(document.GetRootNode());
		ASSERT_NE(copy, nullptr);
		ASSERT_NE(copy->GetInteraction<Toggle>(), nullptr);
		EXPECT_TRUE(copy->GetInteraction<Toggle>()->IsOn());
		EXPECT_EQ(copy->GetInteraction<Toggle>()->GetOnOverrides().Size(), 1u);
		other.UpdateLayout();
		Frame(other);
		EXPECT_FLOAT_EQ(copy->GetChildren()[0]->GetRotation(), 90.0f);
	}

	TEST_F(InteractionTest, TogglesOfAGroupAreExclusive)
	{
		Node*   leftNode = AddNode(_canvas.GetRootNode(), "Left", Vector2(0.0f, 0.0f), Vector2(0.5f, 1.0f));
		Node*   rightNode = AddNode(_canvas.GetRootNode(), "Right", Vector2(0.5f, 0.0f), Vector2(1.0f, 1.0f));
		Toggle& left = leftNode->SetInteraction<Toggle>();
		Toggle& right = rightNode->SetInteraction<Toggle>();
		left.SetGroup("Tabs");
		right.SetGroup("Tabs");
		AssignIds();

		Vector2 leftPoint(-480.0f, 0.0f);
		Vector2 rightPoint(480.0f, 0.0f);
		Click(_canvas, leftPoint);
		EXPECT_TRUE(left.IsOn());
		Click(_canvas, rightPoint);
		EXPECT_TRUE(right.IsOn());
		EXPECT_FALSE(left.IsOn());
		// The one on stays on
		Click(_canvas, rightPoint);
		EXPECT_TRUE(right.IsOn());
	}

	TEST_F(InteractionTest, SliderFollowsThePointerAndPlacesItsFillAndHandle)
	{
		// 200 wide, centered: from x -100 to 100
		Node*   sliderNode = AddNode(_canvas.GetRootNode(), "Slider", Vector2(0.5f, 0.5f), Vector2(0.5f, 0.5f), Vector2(200.0f, 20.0f));
		Node*   fill = AddNode(sliderNode, "Fill", Vector2(0.0f, 0.0f), Vector2(1.0f, 1.0f));
		Node*   handle = AddNode(sliderNode, "Handle", Vector2(0.0f, 0.5f), Vector2(0.0f, 0.5f), Vector2(10.0f, 10.0f));
		Slider& slider = sliderNode->SetInteraction<Slider>();
		slider.SetAction("Volume");
		AssignIds();

		// Referenced as the editor does: by id, through reflection
		GetReference(slider, "Fill").SetLocalId(fill->GetLocalId());
		GetReference(slider, "Handle").SetLocalId(handle->GetLocalId());

		slider.SetMinValue(0.0f);
		slider.SetMaxValue(10.0f);
		slider.SetWholeNumbers(true);

		int actions = 0;
		Canvas::ActionEvent::Slot onAction([&](const String&, Node&) { ++actions; });
		_canvas.GetActionEvent().Connect(onAction);

		// Pressed at 3/4: the value jumps there (7.5, rounded)
		Pointer(_canvas, Vector2(50.0f, 0.0f), false);
		Pointer(_canvas, Vector2(50.0f, 0.0f), true);
		EXPECT_FLOAT_EQ(slider.GetValue(), 8.0f);
		EXPECT_FLOAT_EQ(GetAnchored(fill)->GetAnchorMax().GetX(), 0.8f);
		EXPECT_FLOAT_EQ(GetAnchored(fill)->GetAnchorMin().GetX(), 0.0f);
		EXPECT_FLOAT_EQ(GetAnchored(handle)->GetAnchorMin().GetX(), 0.8f);
		EXPECT_FLOAT_EQ(GetAnchored(handle)->GetAnchorMax().GetX(), 0.8f);
		EXPECT_FLOAT_EQ(GetAnchored(handle)->GetAnchorMin().GetY(), 0.5f); // the other axis kept
		EXPECT_EQ(actions, 1);

		// Dragged at once (no threshold), out of it: clamped
		Pointer(_canvas, Vector2(-300.0f, 40.0f), true);
		EXPECT_FLOAT_EQ(slider.GetValue(), 0.0f);
		EXPECT_EQ(actions, 2);
		Pointer(_canvas, Vector2(-300.0f, 40.0f), false);
		EXPECT_EQ(_canvas.GetPressedNode(), nullptr);

		// The max on the left
		slider.SetDirection(Slider::Direction::RightToLeft);
		EXPECT_FLOAT_EQ(GetAnchored(fill)->GetAnchorMin().GetX(), 1.0f);
		Pointer(_canvas, Vector2(-60.0f, 0.0f), true); // 1/5 from the left: 4/5 of the range
		EXPECT_FLOAT_EQ(slider.GetValue(), 8.0f);
		EXPECT_FLOAT_EQ(GetAnchored(fill)->GetAnchorMin().GetX(), 0.2f);
		EXPECT_FLOAT_EQ(GetAnchored(fill)->GetAnchorMax().GetX(), 1.0f);
		Pointer(_canvas, Vector2(-60.0f, 0.0f), false);

		// Disabled: the pointer doesn't change it
		slider.SetDisabled(true);
		Pointer(_canvas, Vector2(90.0f, 0.0f), true);
		Pointer(_canvas, Vector2(90.0f, 0.0f), false);
		EXPECT_FLOAT_EQ(slider.GetValue(), 8.0f);
	}

	class ScrollViewTest : public InteractionTest
	{
	protected:
		void SetUp() override
		{
			// The viewport over the whole canvas, a content 3000 high at its top, a Button filling the content
			_viewport = AddNode(_canvas.GetRootNode(), "Viewport", Vector2::Zero, Vector2::One);
			_content = AddNode(_viewport, "Content", Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 3000.0f), Vector2(0.5f, 1.0f));
			_buttonNode = AddNode(_content, "Button", Vector2::Zero, Vector2::One);
			_button = &_buttonNode->SetInteraction<Button>();
			_button->GetClickEvent().Connect(_onClick);
			_scrollView = &_viewport->SetInteraction<ScrollView>();
			AssignIds();

			GetReference(*_scrollView, "Content").SetLocalId(_content->GetLocalId());
			_scrollView->SetInertia(false);
			Frame(_canvas);
		}

		float GetOffsetY() const
		{
			return GetAnchored(_content)->GetOffset().GetY();
		}

		// Pressed at from, dragged to to, a frame per step
		void Drag(const Vector2& from, const Vector2& to, bool release = true)
		{
			Pointer(_canvas, from, false);
			Pointer(_canvas, from, true);
			Frame(_canvas);
			Vector2 start = from + Vector2(0.0f, to.GetY() > from.GetY() ? Canvas::DragThreshold + 1.0f : -Canvas::DragThreshold - 1.0f);
			Pointer(_canvas, start, true); // the drag begins
			Frame(_canvas);
			Pointer(_canvas, to, true);
			Frame(_canvas);
			if (release)
			{
				Pointer(_canvas, to, false);
				Frame(_canvas);
			}
		}

		Node*       _viewport = nullptr;
		Node*       _content = nullptr;
		Node*       _buttonNode = nullptr;
		Button*     _button = nullptr;
		ScrollView* _scrollView = nullptr;
		int         _clicks = 0;
		Button::ClickEvent::Slot _onClick = Button::ClickEvent::Slot([this]() { ++_clicks; });
	};

	TEST_F(ScrollViewTest, DragTakesThePressFromAButton)
	{
		Pointer(_canvas, Vector2::Zero, false);
		Pointer(_canvas, Vector2::Zero, true);
		EXPECT_EQ(_canvas.GetPressedNode(), _buttonNode);
		EXPECT_EQ(_button->GetState(), Selectable::State::Pressed);

		// Beyond the threshold: the scroll view takes the press, the button is released
		Pointer(_canvas, Vector2(0.0f, 20.0f), true);
		EXPECT_EQ(_canvas.GetPressedNode(), _viewport);
		EXPECT_NE(_button->GetState(), Selectable::State::Pressed);

		// Up by 100: the content follows
		Pointer(_canvas, Vector2(0.0f, 120.0f), true);
		Frame(_canvas);
		EXPECT_FLOAT_EQ(GetOffsetY(), 100.0f);

		Pointer(_canvas, Vector2(0.0f, 120.0f), false);
		EXPECT_EQ(_clicks, 0);

		// A press without moving still clicks
		Click(_canvas, Vector2::Zero);
		EXPECT_EQ(_clicks, 1);
	}

	TEST_F(ScrollViewTest, ClampedNeverGoesPastTheEdges)
	{
		_scrollView->SetMovementType(ScrollView::MovementType::Clamped);
		// What the pointer moved within the threshold isn't dragged
		Drag(Vector2::Zero, Vector2(0.0f, 300.0f));
		EXPECT_FLOAT_EQ(GetOffsetY(), 300.0f - Canvas::DragThreshold - 1.0f);

		// Down past the top edge: stops there
		Drag(Vector2::Zero, Vector2(0.0f, -900.0f));
		EXPECT_FLOAT_EQ(GetOffsetY(), 0.0f);

		// Up past the bottom one: 3000 - 1080 at most
		Drag(Vector2::Zero, Vector2(0.0f, 5000.0f));
		EXPECT_FLOAT_EQ(GetOffsetY(), 1920.0f);
	}

	TEST_F(ScrollViewTest, ElasticGoesPastTheEdgesThenComesBack)
	{
		Drag(Vector2::Zero, Vector2(0.0f, -400.0f), false);
		EXPECT_LT(GetOffsetY(), 0.0f);
		EXPECT_GT(GetOffsetY(), -400.0f); // resisting
		Pointer(_canvas, Vector2(0.0f, -400.0f), false);
		for (int frame = 0; frame < 120; ++frame)
		{
			Frame(_canvas);
		}
		EXPECT_NEAR(GetOffsetY(), 0.0f, 0.01f);
	}

	TEST_F(ScrollViewTest, WheelScrollsAndInertiaGoesOn)
	{
		_scrollView->SetMovementType(ScrollView::MovementType::Clamped);

		// Towards the user: the content below, so the content up
		Pointer(_canvas, Vector2::Zero, false, Vector2(0.0f, -2.0f));
		Frame(_canvas);
		EXPECT_FLOAT_EQ(GetOffsetY(), 2.0f * _scrollView->GetScrollSensitivity());
		Pointer(_canvas, Vector2::Zero, false, Vector2(0.0f, 10.0f));
		Frame(_canvas);
		EXPECT_FLOAT_EQ(GetOffsetY(), 0.0f);

		// Released while moving: goes on, slower and slower
		_scrollView->SetInertia(true);
		Drag(Vector2::Zero, Vector2(0.0f, 200.0f), false);
		Pointer(_canvas, Vector2(0.0f, 200.0f), false);
		float released = GetOffsetY();
		Frame(_canvas);
		float first = GetOffsetY() - released;
		Frame(_canvas);
		float second = GetOffsetY() - released - first;
		EXPECT_GT(first, 0.0f);
		EXPECT_GT(second, 0.0f);
		EXPECT_LT(second, first);

		// Pressed: stops
		Pointer(_canvas, Vector2::Zero, true);
		float held = GetOffsetY();
		Frame(_canvas);
		EXPECT_FLOAT_EQ(GetOffsetY(), held);
		Pointer(_canvas, Vector2::Zero, false);
	}

	TEST_F(ScrollViewTest, NormalizedPosition)
	{
		EXPECT_FLOAT_EQ(_scrollView->GetNormalizedPosition().GetY(), 1.0f); // at the top

		_scrollView->SetNormalizedPosition(Vector2(0.0f, 0.0f));
		Frame(_canvas);
		EXPECT_FLOAT_EQ(GetOffsetY(), 1920.0f);
		EXPECT_FLOAT_EQ(_scrollView->GetNormalizedPosition().GetY(), 0.0f);

		_scrollView->SetNormalizedPosition(Vector2(0.0f, 0.5f));
		Frame(_canvas);
		EXPECT_FLOAT_EQ(GetOffsetY(), 960.0f);
	}
}

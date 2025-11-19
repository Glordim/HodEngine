#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/Layout/LayoutElement.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/Padding.hpp"
#include "HodEngine/UI/Rebuildable.hpp"

#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Game/Entity.hpp>

namespace hod::ui
{
	/// @brief
	class HOD_UI_API Layout : public Rebuildable
	{
		REFLECTED_CLASS(Layout, Rebuildable)

	public:
		enum Axis
		{
			Horizontal = 0,
			Vertical,
		};

		REFLECTED_ENUM(HOD_UI_API, Axis);

		enum Alignment
		{
			TopLeft = 0,
			TopCenter,
			TopRight,
			MiddleLeft,
			MiddleCenter,
			MiddleRight,
			BottomLeft,
			BottomCenter,
			BottomRight,
		};

		REFLECTED_ENUM(HOD_UI_API, Alignment);

		enum ChangedSizeProperty
		{
			TotalSizeX = 0,
			TotalSizeY
		};

		using SizeChangedEvent = Event<ChangedSizeProperty>;

	public:
		Layout();

		const Padding& GetPadding() const;
		void           SetPadding(const Padding& padding);

		Alignment GetAlignment() const;
		void      SetAlignment(Alignment alignment);

		const Vector2& GetTotalMinSize() const;
		const Vector2& GetTotalPreferredSize() const;
		const Vector2& GetTotalFlexibleSize() const;

		bool Rebuild() override;

		SizeChangedEvent& GetSizeChangedEvent();

	protected:
		void OnEnable() override;
		void OnDisable() override;

		const Vector<WeakPtr<Node>>& GetDrivenNodes() const;
		virtual void                 ComputeChildrenPositionAndSize() = 0;

		float GetAlignmentMultiplier(Axis axis);
		float GetStartOffset(Axis axis, float requiredSpaceWithoutPadding, float availableSpace);
		void  SetChildAlongAxis(Node* node, Axis axis, float pos);
		void  SetChildAlongAxisWithScale(Node* node, Axis axis, float pos, float scaleFactor);
		void  SetChildAlongAxisWithScale(Node* node, Axis axis, float pos, float size, float scaleFactor);

		void SetTotalSizeX(float minSize, float preferredSize, float flexibleSize);
		void SetTotalSizeY(float minSize, float preferredSize, float flexibleSize);

	protected:
		Padding   _padding;
		Alignment _alignment;

	private:
		void OnDrivenNodeChanged();
		void OnDrivenNodeLayoutElementChanged(LayoutElement::ChangedProperty changedProperty);
		void OnChildrenChanged();

	private:
		bool                  _drivenNodesDirty = true;
		Vector<WeakPtr<Node>> _drivenNodes;

		Node::PropertyChangedEvent::Slot          _onDrivenNodeChangedSlot;
		LayoutElement::PropertyChangedEvent::Slot _onDrivenNodeLayoutElementChangedSlot;
		game::Entity::ChildrenChangedEvent::Slot  _onChildrenChangedSlot;

		Vector2 _totalMinSize;
		Vector2 _totalPreferredSize;
		Vector2 _totalFlexibleSize;

		SizeChangedEvent _sizeChangedEvent;
	};
}

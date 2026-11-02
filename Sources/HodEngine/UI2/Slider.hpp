#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/NodeRef.hpp"
#include "HodEngine/UI2/Selectable.hpp"

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/String.hpp>

namespace hod::inline ui2
{
	/// @brief A value between a min and a max, set by pressing or dragging along it (a volume, a progress bar when disabled).
	/// It shows the value through two optional nodes of its prefab, placed by anchors (AnchoredLayoutParams) within their
	/// parent: the fill, spanning from the start to the value, and the handle, anchored on the value. The pointer's position is
	/// read along the handle's parent (the fill's without handle, the slider's own node without either). Its look follows its
	/// state (see Selectable).
	///
	/// A change emits the value changed event, and, from the pointer, the action (see Canvas::GetActionEvent: gameplay reads
	/// GetValue() from the node), as long as it is dragged.
	class HOD_UI2_API Slider : public Selectable
	{
		REFLECTED_CLASS(Slider, Selectable)

	public:

		// Where the min is, the max at the opposite end
		enum class Direction : uint8_t
		{
			LeftToRight,
			RightToLeft,
			BottomToTop,
			TopToBottom,
		};
		REFLECTED_ENUM(HOD_UI2_API, Direction);

		using ValueChangedEvent = Event<float>;

	public:

		float				GetMinValue() const;
		void				SetMinValue(float minValue);

		float				GetMaxValue() const;
		void				SetMaxValue(float maxValue);

		// Kept between min and max (rounded when WholeNumbers). Sets it with the value changed event, not the action.
		float				GetValue() const;
		void				SetValue(float value);
		// The value between min (0) and max (1)
		float				GetNormalizedValue() const;
		void				SetNormalizedValue(float normalizedValue);

		bool				GetWholeNumbers() const;
		void				SetWholeNumbers(bool wholeNumbers);

		Direction			GetDirection() const;
		void				SetDirection(Direction direction);

		// Name the canvas emits when the pointer changes the value (see Canvas::GetActionEvent). Empty: no action.
		const String&		GetAction() const;
		void				SetAction(const String& action);

		ValueChangedEvent&	GetValueChangedEvent();

		// Press (the value jumps under the pointer) then drag, at once: no drag threshold
		bool				OnPointerEvent(const PointerEvent& event) override;
		bool				UsesDragThreshold() const override;
		void				Update(float deltaTime) override;
		void				RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers) override;

	private:

		// The reflected setters: the value as authored, without event
		void				SetValueProperty(float value);
		// Clamped and rounded, then shown. Returns whether it changed.
		bool				ApplyValue(float value);
		// Places the fill and the handle for the value
		void				UpdateVisuals();
		// The value under the pointer (canvas position), along the track
		bool				ComputeValueAt(const Vector2& canvasPosition, float& outValue);
		Node*				FindTrack();

	private:

		float				_minValue = 0.0f;
		float				_maxValue = 1.0f;
		float				_value = 0.0f;
		bool				_wholeNumbers = false;
		Direction			_direction = Direction::LeftToRight;
		NodeRef<Node>		_fill;
		NodeRef<Node>		_handle;
		String				_action;

		bool				_visualsApplied = false;
		bool				_dragging = false;

		ValueChangedEvent	_valueChangedEvent;
	};
}

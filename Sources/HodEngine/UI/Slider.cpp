#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Slider.hpp"
#include "HodEngine/UI/AnchoredLayoutParams.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"
#include "HodEngine/UI/PointerEvent.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <cmath>

namespace hod::inline ui
{
	DESCRIBE_REFLECTED_ENUM(Slider::Direction, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Slider::Direction::LeftToRight, "LeftToRight");
		reflectionDescriptor.AddEnumValue(Slider::Direction::RightToLeft, "RightToLeft");
		reflectionDescriptor.AddEnumValue(Slider::Direction::BottomToTop, "BottomToTop");
		reflectionDescriptor.AddEnumValue(Slider::Direction::TopToBottom, "TopToBottom");
	}

	DESCRIBE_REFLECTED_CLASS(Slider, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_TUNE_VARIANT);

		AddPropertyT(reflectionDescriptor, &Slider::_minValue, "MinValue", &Slider::SetMinValue);
		AddPropertyT(reflectionDescriptor, &Slider::_maxValue, "MaxValue", &Slider::SetMaxValue);
		AddPropertyT(reflectionDescriptor, &Slider::_value, "Value", &Slider::SetValueProperty);
		AddPropertyT(reflectionDescriptor, &Slider::_wholeNumbers, "WholeNumbers", &Slider::SetWholeNumbers);
		AddPropertyT(reflectionDescriptor, &Slider::_direction, "Direction", &Slider::SetDirection);
		AddPropertyT(reflectionDescriptor, &Slider::_fill, "Fill");
		AddPropertyT(reflectionDescriptor, &Slider::_handle, "Handle");
		AddPropertyT(reflectionDescriptor, &Slider::_action, "Action", &Slider::SetAction);
	}

	namespace
	{
		bool IsHorizontal(Slider::Direction direction)
		{
			return direction == Slider::Direction::LeftToRight || direction == Slider::Direction::RightToLeft;
		}

		bool IsReversed(Slider::Direction direction)
		{
			return direction == Slider::Direction::RightToLeft || direction == Slider::Direction::TopToBottom;
		}

		Vector2 WithAxis(const Vector2& vector, bool horizontal, float value)
		{
			return horizontal ? Vector2(value, vector.GetY()) : Vector2(vector.GetX(), value);
		}

		// The node of that id in owner's prefab, null for none (no warning: it may not be assigned yet)
		Node* FindReferenced(Node& owner, const NodeRefBase& reference)
		{
			return reference.GetLocalId() != 0 ? owner.GetScopeRoot()->FindByLocalId(reference.GetLocalId()) : nullptr;
		}
	}

	/// @brief
	/// @return
	float Slider::GetMinValue() const
	{
		return _minValue;
	}

	/// @brief
	/// @param minValue
	void Slider::SetMinValue(float minValue)
	{
		_minValue = minValue;
		ApplyValue(_value);
	}

	/// @brief
	/// @return
	float Slider::GetMaxValue() const
	{
		return _maxValue;
	}

	/// @brief
	/// @param maxValue
	void Slider::SetMaxValue(float maxValue)
	{
		_maxValue = maxValue;
		ApplyValue(_value);
	}

	/// @brief
	/// @return
	float Slider::GetValue() const
	{
		return _value;
	}

	/// @brief
	/// @param value
	void Slider::SetValue(float value)
	{
		if (ApplyValue(value))
		{
			_valueChangedEvent.Emit(_value);
		}
	}

	/// @brief
	/// @param value
	void Slider::SetValueProperty(float value)
	{
		ApplyValue(value);
	}

	/// @brief
	/// @return
	float Slider::GetNormalizedValue() const
	{
		float range = _maxValue - _minValue;
		return range != 0.0f ? Clamp((_value - _minValue) / range, 0.0f, 1.0f) : 0.0f;
	}

	/// @brief
	/// @param normalizedValue
	void Slider::SetNormalizedValue(float normalizedValue)
	{
		SetValue(_minValue + (_maxValue - _minValue) * Clamp(normalizedValue, 0.0f, 1.0f));
	}

	/// @brief
	/// @return
	bool Slider::GetWholeNumbers() const
	{
		return _wholeNumbers;
	}

	/// @brief
	/// @param wholeNumbers
	void Slider::SetWholeNumbers(bool wholeNumbers)
	{
		_wholeNumbers = wholeNumbers;
		ApplyValue(_value);
	}

	/// @brief
	/// @return
	Slider::Direction Slider::GetDirection() const
	{
		return _direction;
	}

	/// @brief
	/// @param direction
	void Slider::SetDirection(Direction direction)
	{
		_direction = direction;
		UpdateVisuals();
	}

	/// @brief
	/// @return
	const String& Slider::GetAction() const
	{
		return _action;
	}

	/// @brief
	/// @param action
	void Slider::SetAction(const String& action)
	{
		_action = action;
	}

	/// @brief
	/// @return
	Slider::ValueChangedEvent& Slider::GetValueChangedEvent()
	{
		return _valueChangedEvent;
	}

	/// @brief
	/// @param event
	/// @return
	bool Slider::OnPointerEvent(const PointerEvent& event)
	{
		bool fromPointer = false;
		switch (event._type)
		{
			case PointerEvent::Type::DragBegin:
			{
				_dragging = (IsDisabled() == false);
				fromPointer = _dragging;
				Selectable::OnPointerEvent(event);
				if (fromPointer == false)
				{
					return true; // taken, not to scroll what holds it
				}
			}
			break;

			case PointerEvent::Type::Drag: fromPointer = _dragging; break;

			case PointerEvent::Type::DragEnd: _dragging = false; return true;

			case PointerEvent::Type::Press:
			{
				Selectable::OnPointerEvent(event);
				fromPointer = (IsDisabled() == false);
				if (fromPointer == false)
				{
					return true;
				}
			}
			break;

			default: return Selectable::OnPointerEvent(event);
		}

		float value = 0.0f;
		if (fromPointer && ComputeValueAt(event._position, value) && ApplyValue(value))
		{
			_valueChangedEvent.Emit(_value);
			if (_action.Empty() == false && event._canvas != nullptr && GetNode() != nullptr)
			{
				event._canvas->EmitAction(_action, *GetNode());
			}
		}
		return true;
	}

	/// @brief
	/// @return
	bool Slider::UsesDragThreshold() const
	{
		return false;
	}

	/// @brief
	/// @param deltaTime
	void Slider::Update(float deltaTime)
	{
		if (_visualsApplied == false)
		{
			UpdateVisuals();
		}
		Selectable::Update(deltaTime);
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void Slider::RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		Selectable::RemapTargets(oldToNew, keepOthers);
		_fill.Remap(oldToNew, keepOthers);
		_handle.Remap(oldToNew, keepOthers);
	}

	/// @brief
	/// @param value
	/// @return
	bool Slider::ApplyValue(float value)
	{
		float clamped = Clamp(value, Min(_minValue, _maxValue), Max(_minValue, _maxValue));
		if (_wholeNumbers)
		{
			clamped = std::round(clamped);
		}
		bool changed = (clamped != _value);
		_value = clamped;
		UpdateVisuals();
		return changed;
	}

	/// @brief
	void Slider::UpdateVisuals()
	{
		if (GetNode() == nullptr)
		{
			return;
		}
		_visualsApplied = true;

		bool  horizontal = IsHorizontal(_direction);
		bool  reversed = IsReversed(_direction);
		float normalized = GetNormalizedValue();

		// Only what changed: the anchors mark the layout dirty
		if (Node* fill = FindReferenced(*GetNode(), _fill))
		{
			if (AnchoredLayoutParams* params = LayoutParams::Cast<AnchoredLayoutParams>(fill->GetLayoutParams()))
			{
				Vector2 anchorMin = WithAxis(params->GetAnchorMin(), horizontal, reversed ? 1.0f - normalized : 0.0f);
				Vector2 anchorMax = WithAxis(params->GetAnchorMax(), horizontal, reversed ? 1.0f : normalized);
				if (anchorMin != params->GetAnchorMin())
				{
					params->SetAnchorMin(anchorMin);
				}
				if (anchorMax != params->GetAnchorMax())
				{
					params->SetAnchorMax(anchorMax);
				}
			}
		}
		if (Node* handle = FindReferenced(*GetNode(), _handle))
		{
			if (AnchoredLayoutParams* params = LayoutParams::Cast<AnchoredLayoutParams>(handle->GetLayoutParams()))
			{
				float   position = reversed ? 1.0f - normalized : normalized;
				Vector2 anchorMin = WithAxis(params->GetAnchorMin(), horizontal, position);
				Vector2 anchorMax = WithAxis(params->GetAnchorMax(), horizontal, position);
				if (anchorMin != params->GetAnchorMin())
				{
					params->SetAnchorMin(anchorMin);
				}
				if (anchorMax != params->GetAnchorMax())
				{
					params->SetAnchorMax(anchorMax);
				}
			}
		}
	}

	/// @brief
	/// @return
	Node* Slider::FindTrack()
	{
		if (Node* handle = FindReferenced(*GetNode(), _handle); handle != nullptr && handle->GetParent() != nullptr)
		{
			return handle->GetParent();
		}
		if (Node* fill = FindReferenced(*GetNode(), _fill); fill != nullptr && fill->GetParent() != nullptr)
		{
			return fill->GetParent();
		}
		return GetNode();
	}

	/// @brief
	/// @param canvasPosition
	/// @param outValue
	/// @return
	bool Slider::ComputeValueAt(const Vector2& canvasPosition, float& outValue)
	{
		if (GetNode() == nullptr)
		{
			return false;
		}
		Node*   track = FindTrack();
		Vector2 size = track->GetSize();
		bool    horizontal = IsHorizontal(_direction);
		float   extent = horizontal ? size.GetX() : size.GetY();
		if (extent <= 0.0f || track->GetScale().GetX() == 0.0f || track->GetScale().GetY() == 0.0f)
		{
			return false;
		}

		// In the track's own space, its rect centered on the origin (y up)
		Vector4 local = Matrix4::Inverse(track->ComputeCanvasMatrix()) * Vector4(canvasPosition.GetX(), canvasPosition.GetY(), 0.0f, 1.0f);
		float   normalized = Clamp(((horizontal ? local.GetX() : local.GetY()) + extent * 0.5f) / extent, 0.0f, 1.0f);
		if (IsReversed(_direction))
		{
			normalized = 1.0f - normalized;
		}
		outValue = _minValue + (_maxValue - _minValue) * normalized;
		return true;
	}
}

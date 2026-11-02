#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/ScrollView.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/PointerEvent.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitIcon.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <cmath>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(ScrollView::MovementType, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(ScrollView::MovementType::Clamped, "Clamped");
		reflectionDescriptor.AddEnumValue(ScrollView::MovementType::Elastic, "Elastic");
	}

	DESCRIBE_REFLECTED_CLASS(ScrollView, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitIcon>(ICON_MDI_PAN_VERTICAL);

		AddPropertyT(reflectionDescriptor, &ScrollView::_content, "Content");
		AddPropertyT(reflectionDescriptor, &ScrollView::_horizontal, "Horizontal", &ScrollView::SetHorizontal);
		AddPropertyT(reflectionDescriptor, &ScrollView::_vertical, "Vertical", &ScrollView::SetVertical);
		AddPropertyT(reflectionDescriptor, &ScrollView::_movementType, "MovementType", &ScrollView::SetMovementType);
		AddPropertyT(reflectionDescriptor, &ScrollView::_elasticity, "Elasticity", &ScrollView::SetElasticity);
		AddPropertyT(reflectionDescriptor, &ScrollView::_inertia, "Inertia", &ScrollView::SetInertia);
		AddPropertyT(reflectionDescriptor, &ScrollView::_decelerationRate, "DecelerationRate", &ScrollView::SetDecelerationRate);
		AddPropertyT(reflectionDescriptor, &ScrollView::_scrollSensitivity, "ScrollSensitivity", &ScrollView::SetScrollSensitivity);
	}

	namespace
	{
		// One axis of ScrollView::ComputeCorrection. A content smaller than the viewport sticks to its start (alignMax: its top).
		float ComputeAxisCorrection(float contentMin, float contentMax, float viewMin, float viewMax, bool alignMax)
		{
			if (contentMax - contentMin <= viewMax - viewMin)
			{
				return alignMax ? viewMax - contentMax : viewMin - contentMin;
			}
			if (contentMin > viewMin)
			{
				return viewMin - contentMin;
			}
			if (contentMax < viewMax)
			{
				return viewMax - contentMax;
			}
			return 0.0f;
		}
	}

	/// @brief
	/// @return
	bool ScrollView::GetHorizontal() const
	{
		return _horizontal;
	}

	/// @brief
	/// @param horizontal
	void ScrollView::SetHorizontal(bool horizontal)
	{
		_horizontal = horizontal;
	}

	/// @brief
	/// @return
	bool ScrollView::GetVertical() const
	{
		return _vertical;
	}

	/// @brief
	/// @param vertical
	void ScrollView::SetVertical(bool vertical)
	{
		_vertical = vertical;
	}

	/// @brief
	/// @return
	ScrollView::MovementType ScrollView::GetMovementType() const
	{
		return _movementType;
	}

	/// @brief
	/// @param movementType
	void ScrollView::SetMovementType(MovementType movementType)
	{
		_movementType = movementType;
	}

	/// @brief
	/// @return
	float ScrollView::GetElasticity() const
	{
		return _elasticity;
	}

	/// @brief
	/// @param elasticity
	void ScrollView::SetElasticity(float elasticity)
	{
		_elasticity = elasticity;
	}

	/// @brief
	/// @return
	bool ScrollView::GetInertia() const
	{
		return _inertia;
	}

	/// @brief
	/// @param inertia
	void ScrollView::SetInertia(bool inertia)
	{
		_inertia = inertia;
	}

	/// @brief
	/// @return
	float ScrollView::GetDecelerationRate() const
	{
		return _decelerationRate;
	}

	/// @brief
	/// @param decelerationRate
	void ScrollView::SetDecelerationRate(float decelerationRate)
	{
		_decelerationRate = decelerationRate;
	}

	/// @brief
	/// @return
	float ScrollView::GetScrollSensitivity() const
	{
		return _scrollSensitivity;
	}

	/// @brief
	/// @param scrollSensitivity
	void ScrollView::SetScrollSensitivity(float scrollSensitivity)
	{
		_scrollSensitivity = scrollSensitivity;
	}

	/// @brief
	/// @return
	Vector2 ScrollView::GetNormalizedPosition()
	{
		Node* content = FindContent();
		if (content == nullptr)
		{
			return Vector2(0.0f, 1.0f);
		}

		Vector2 contentMin, contentMax, viewMin, viewMax;
		ComputeBounds(*content, contentMin, contentMax, viewMin, viewMax);
		auto axis = [](float contentMin, float contentMax, float viewMin, float viewMax, float smaller)
		{
			float range = (contentMax - contentMin) - (viewMax - viewMin);
			return range > 0.0f ? Clamp((viewMin - contentMin) / range, 0.0f, 1.0f) : smaller;
		};
		return Vector2(axis(contentMin.GetX(), contentMax.GetX(), viewMin.GetX(), viewMax.GetX(), 0.0f),
		               axis(contentMin.GetY(), contentMax.GetY(), viewMin.GetY(), viewMax.GetY(), 1.0f));
	}

	/// @brief
	/// @param normalizedPosition
	void ScrollView::SetNormalizedPosition(const Vector2& normalizedPosition)
	{
		_hasPendingPosition = true;
		_pendingPosition = Vector2(Clamp(normalizedPosition.GetX(), 0.0f, 1.0f), Clamp(normalizedPosition.GetY(), 0.0f, 1.0f));
	}

	/// @brief
	/// @return
	const Vector2& ScrollView::GetVelocity() const
	{
		return _velocity;
	}

	/// @brief
	void ScrollView::StopMovement()
	{
		_velocity = Vector2::Zero;
	}

	/// @brief
	/// @return
	ScrollView::ScrolledEvent& ScrollView::GetScrolledEvent()
	{
		return _scrolledEvent;
	}

	/// @brief
	/// @param event
	/// @return
	bool ScrollView::OnPointerEvent(const PointerEvent& event)
	{
		switch (event._type)
		{
			// Held (even through a Button in it): the content stops under the pointer
			case PointerEvent::Type::PressWithin: StopMovement(); return false;
			case PointerEvent::Type::Press: return true;
			case PointerEvent::Type::Release: return true;

			case PointerEvent::Type::DragBegin:
			{
				if ((_horizontal == false && _vertical == false) || FindContent() == nullptr)
				{
					return false;
				}
				_dragging = true;
				StopMovement();
				return true;
			}

			case PointerEvent::Type::Drag:
			{
				if (Node* content = FindContent(); content != nullptr && _dragging)
				{
					_pendingDrag += ToViewport(*content, event._delta);
				}
				return true;
			}

			case PointerEvent::Type::DragEnd: _dragging = false; return true;

			case PointerEvent::Type::Scroll:
			{
				if ((_horizontal == false && _vertical == false) || FindContent() == nullptr)
				{
					return false;
				}
				_pendingScroll += event._scroll;
				return true;
			}

			default: return false;
		}
	}

	/// @brief
	/// @param deltaTime
	void ScrollView::Update(float deltaTime)
	{
		Node* content = FindContent();
		if (content == nullptr)
		{
			_pendingDrag = Vector2::Zero;
			_pendingScroll = Vector2::Zero;
			_movedSinceLayout = Vector2::Zero;
			return;
		}
		AnchoredLayoutParams* params = LayoutParams::Cast<AnchoredLayoutParams>(content->GetLayoutParams());

		Vector2 contentMin, contentMax, viewMin, viewMax;
		ComputeBounds(*content, contentMin, contentMax, viewMin, viewMax);
		Vector2 move = Vector2::Zero;

		if (_hasPendingPosition)
		{
			_hasPendingPosition = false;
			Vector2 range = (contentMax - contentMin) - (viewMax - viewMin);
			Vector2 target(viewMin.GetX() - _pendingPosition.GetX() * Max(range.GetX(), 0.0f), viewMin.GetY() - _pendingPosition.GetY() * Max(range.GetY(), 0.0f));
			move += KeepAxes(target - contentMin);
			StopMovement();
		}

		if (_dragging)
		{
			Vector2 delta = KeepAxes(_pendingDrag);
			// Elastic: past its edges, the content follows the pointer by half
			if (_movementType == MovementType::Elastic)
			{
				Vector2 correction = ComputeCorrection(contentMin + move + delta, contentMax + move + delta, viewMin, viewMax);
				delta = Vector2(correction.GetX() != 0.0f ? delta.GetX() * 0.5f : delta.GetX(), correction.GetY() != 0.0f ? delta.GetY() * 0.5f : delta.GetY());
			}
			move += delta;
			if (deltaTime > 0.0f)
			{
				// Smoothed: the speed it goes on at once released
				_velocity = Vector2::Lerp(_velocity, delta / deltaTime, Min(deltaTime * 10.0f, 1.0f));
			}
		}
		else
		{
			Vector2 correction = ComputeCorrection(contentMin + move, contentMax + move, viewMin, viewMax);
			float   velocity[2] = {_velocity.GetX(), _velocity.GetY()};
			float   step[2] = {0.0f, 0.0f};
			for (uint32_t axis = 0; axis < 2; ++axis)
			{
				float axisCorrection = (axis == 0) ? correction.GetX() : correction.GetY();
				if (axisCorrection != 0.0f && _movementType == MovementType::Elastic)
				{
					// Pulled back, about the elasticity's duration
					velocity[axis] = 0.0f;
					step[axis] = std::fabs(axisCorrection) < 0.5f ? axisCorrection : axisCorrection * (1.0f - std::exp(-deltaTime / Max(_elasticity, 0.001f) * 3.0f));
				}
				else if (_inertia)
				{
					velocity[axis] *= std::pow(Clamp(_decelerationRate, 0.0f, 1.0f), deltaTime);
					if (std::fabs(velocity[axis]) < 1.0f)
					{
						velocity[axis] = 0.0f;
					}
					step[axis] = velocity[axis] * deltaTime;
				}
				else
				{
					velocity[axis] = 0.0f;
				}
			}
			_velocity = KeepAxes(Vector2(velocity[0], velocity[1]));
			move += KeepAxes(Vector2(step[0], step[1]));
		}

		// The wheel: away from the user shows what is above (or on the left, scrolling horizontally only)
		if (_pendingScroll != Vector2::Zero)
		{
			Vector2 scroll = _pendingScroll * _scrollSensitivity;
			Vector2 wheelMove = (_vertical == false) ? Vector2(scroll.GetY() - scroll.GetX(), 0.0f) : Vector2(-scroll.GetX(), -scroll.GetY());
			move += KeepAxes(wheelMove);
			StopMovement();
		}
		_pendingDrag = Vector2::Zero;
		_pendingScroll = Vector2::Zero;

		// Clamped: never past the edges (Elastic: pulled back afterwards)
		if (_movementType == MovementType::Clamped)
		{
			Vector2 correction = ComputeCorrection(contentMin + move, contentMax + move, viewMin, viewMax);
			move += correction;
			_velocity = Vector2(correction.GetX() != 0.0f ? 0.0f : _velocity.GetX(), correction.GetY() != 0.0f ? 0.0f : _velocity.GetY());
		}

		if (move != Vector2::Zero)
		{
			params->SetOffset(params->GetOffset() + move);
			_movedSinceLayout += move;
			_scrolledEvent.Emit(GetNormalizedPosition());
		}
	}

	/// @brief
	/// @param oldToNew
	/// @param keepOthers
	void ScrollView::RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers)
	{
		_content.Remap(oldToNew, keepOthers);
	}

	/// @brief
	/// @return
	Node* ScrollView::FindContent()
	{
		if (GetNode() == nullptr || _content.GetLocalId() == 0)
		{
			return nullptr;
		}
		Node* content = GetNode()->GetScopeRoot()->FindByLocalId(_content.GetLocalId());
		if (content == nullptr || content->GetParent() == nullptr || LayoutParams::Cast<AnchoredLayoutParams>(content->GetLayoutParams()) == nullptr)
		{
			return nullptr;
		}

		// Laid out since the last move: the position has it
		if (content->GetPosition() != _laidOutPosition)
		{
			_laidOutPosition = content->GetPosition();
			_movedSinceLayout = Vector2::Zero;
		}
		return content;
	}

	/// @brief
	/// @param content
	/// @param outContentMin
	/// @param outContentMax
	/// @param outViewMin
	/// @param outViewMax
	void ScrollView::ComputeBounds(Node& content, Vector2& outContentMin, Vector2& outContentMax, Vector2& outViewMin, Vector2& outViewMax) const
	{
		Vector2 center = content.GetPosition() + _movedSinceLayout;
		Vector2 halfSize = content.GetSize() * 0.5f;
		outContentMin = center - halfSize;
		outContentMax = center + halfSize;

		// The viewport's content rect, centered on its origin
		Vector2 halfView = content.GetParent()->GetSize() * 0.5f;
		outViewMin = -halfView;
		outViewMax = halfView;
	}

	/// @brief
	/// @param contentMin
	/// @param contentMax
	/// @param viewMin
	/// @param viewMax
	/// @return
	Vector2 ScrollView::ComputeCorrection(const Vector2& contentMin, const Vector2& contentMax, const Vector2& viewMin, const Vector2& viewMax) const
	{
		return KeepAxes(Vector2(ComputeAxisCorrection(contentMin.GetX(), contentMax.GetX(), viewMin.GetX(), viewMax.GetX(), false),
		                        ComputeAxisCorrection(contentMin.GetY(), contentMax.GetY(), viewMin.GetY(), viewMax.GetY(), true)));
	}

	/// @brief
	/// @param content
	/// @param canvasDelta
	/// @return
	Vector2 ScrollView::ToViewport(Node& content, const Vector2& canvasDelta) const
	{
		Node* viewport = content.GetParent();
		if (viewport->GetScale().GetX() == 0.0f || viewport->GetScale().GetY() == 0.0f)
		{
			return Vector2::Zero;
		}
		// A direction: not moved by the translation (w = 0)
		Vector4 local = Matrix4::Inverse(viewport->ComputeCanvasMatrix()) * Vector4(canvasDelta.GetX(), canvasDelta.GetY(), 0.0f, 0.0f);
		return Vector2(local.GetX(), local.GetY());
	}

	/// @brief
	/// @param vector
	/// @return
	Vector2 ScrollView::KeepAxes(const Vector2& vector) const
	{
		return Vector2(_horizontal ? vector.GetX() : 0.0f, _vertical ? vector.GetY() : 0.0f);
	}
}

#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Math/Rect.hpp>

#include <cmath>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(AnchoredLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_anchorMin, "AnchorMin", &AnchoredLayoutParams::SetAnchorMin);
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_anchorMax, "AnchorMax", &AnchoredLayoutParams::SetAnchorMax);
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_alignment, "Alignment", &AnchoredLayoutParams::SetAlignment);
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_offset, "Offset", &AnchoredLayoutParams::SetOffset);
	}

	namespace
	{
		// One axis of the placement, the same math for both: Y-up, so an axis' "start" is the left or the bottom.
		struct Axis
		{
			float _anchorMin;
			float _anchorMax;
			float _alignment;
			float _offset;
			float _sizeDelta; // Size on that axis, only read when it is stretched

			bool IsStretched() const
			{
				return _anchorMin != _anchorMax;
			}

			// Edges of the anchors, relative to the center of the parent's content rect
			float AnchorMinPosition(float parentSize) const
			{
				return (_anchorMin - 0.5f) * parentSize;
			}

			float AnchorMaxPosition(float parentSize) const
			{
				return (_anchorMax - 0.5f) * parentSize;
			}

			// Where the Alignment point goes before Offset: the anchor point, or the same point between the anchors when stretched
			float AlignmentPosition(float parentSize) const
			{
				return AnchorMinPosition(parentSize) + (AnchorMaxPosition(parentSize) - AnchorMinPosition(parentSize)) * _alignment;
			}

			// Stretched: the anchors' span grown by the size delta
			float StretchedSize(float parentSize) const
			{
				return parentSize * (_anchorMax - _anchorMin) + _sizeDelta;
			}

			// Center of the child, relative to the center of the parent's content rect
			float Center(float parentSize, float size) const
			{
				if (IsStretched())
				{
					return AlignmentPosition(parentSize) + _offset - (_alignment - 0.5f) * StretchedSize(parentSize);
				}
				return AlignmentPosition(parentSize) + _offset - (_alignment - 0.5f) * size;
			}
		};

		Axis MakeAxisX(const Vector2& anchorMin, const Vector2& anchorMax, const Vector2& alignment, const Vector2& offset, const Vector2& size)
		{
			return Axis {anchorMin.GetX(), anchorMax.GetX(), alignment.GetX(), offset.GetX(), size.GetX()};
		}

		Axis MakeAxisY(const Vector2& anchorMin, const Vector2& anchorMax, const Vector2& alignment, const Vector2& offset, const Vector2& size)
		{
			return Axis {anchorMin.GetY(), anchorMax.GetY(), alignment.GetY(), offset.GetY(), size.GetY()};
		}
	}

	bool AnchoredLayoutParams::DependsOnParentWidth() const
	{
		return IsStretchedX();
	}

	bool AnchoredLayoutParams::DependsOnParentHeight() const
	{
		return IsStretchedY();
	}

	bool AnchoredLayoutParams::IsStretchedX() const
	{
		return _anchorMin.GetX() != _anchorMax.GetX();
	}

	bool AnchoredLayoutParams::IsStretchedY() const
	{
		return _anchorMin.GetY() != _anchorMax.GetY();
	}

	/// @brief Stretched axes: the anchors' span plus the size delta. The others: the child's own size.
	/// Bounded by MinSize/MaxSize either way.
	/// @param availableSize
	/// @param contentSize
	/// @return
	Vector2 AnchoredLayoutParams::ComputeSize(const Vector2& availableSize, const Vector2& contentSize) const
	{
		Vector2 size = ResolveSize(contentSize);

		Axis x = MakeAxisX(_anchorMin, _anchorMax, _alignment, _offset, GetSize());
		Axis y = MakeAxisY(_anchorMin, _anchorMax, _alignment, _offset, GetSize());
		if (x.IsStretched())
		{
			size.SetX(Clamp(x.StretchedSize(availableSize.GetX()), GetMinSize().GetX(), GetMaxSize().GetX()));
		}
		if (y.IsStretched())
		{
			size.SetY(Clamp(y.StretchedSize(availableSize.GetY()), GetMinSize().GetY(), GetMaxSize().GetY()));
		}
		return size;
	}

	bool AnchoredLayoutParams::TryComputePosition(const Rect& parentContentRect, const Vector2& size, Vector2& outPosition) const
	{
		Axis x = MakeAxisX(_anchorMin, _anchorMax, _alignment, _offset, GetSize());
		Axis y = MakeAxisY(_anchorMin, _anchorMax, _alignment, _offset, GetSize());

		Vector2 parentCenter = parentContentRect._position + parentContentRect._size * 0.5f;
		outPosition = parentCenter + Vector2(x.Center(parentContentRect._size.GetX(), size.GetX()), y.Center(parentContentRect._size.GetY(), size.GetY()));
		return true;
	}

	/// @brief
	/// @param parentSize
	/// @param center
	/// @param size
	void AnchoredLayoutParams::PlaceRect(const Vector2& parentSize, const Vector2& center, const Vector2& size)
	{
		Vector2 offset = _offset;
		Vector2 ownSize = GetSize();
		Node*   node = GetNode();

		// One axis of it, written back by the caller: the offset, and the size delta on a stretched axis or the size/mode on the others.
		auto place = [&](const Axis& axis, float parentExtent, float centerValue, float sizeValue, float currentSize, SizeMode mode, float& outOffset, float& outOwnSize,
		                 SizeMode& outMode)
		{
			if (axis.IsStretched())
			{
				outOwnSize = sizeValue - parentExtent * (axis._anchorMax - axis._anchorMin);
			}
			else if (mode == SizeMode::Fixed || std::abs(sizeValue - currentSize) > 0.001f)
			{
				outMode = SizeMode::Fixed;
				outOwnSize = sizeValue;
			}
			outOffset = centerValue - axis.AlignmentPosition(parentExtent) + (axis._alignment - 0.5f) * sizeValue;
		};

		Vector2 currentSize = node != nullptr ? node->GetSize() : Vector2(-1.0f, -1.0f);

		float    offsetX = offset.GetX(), offsetY = offset.GetY();
		float    ownSizeX = ownSize.GetX(), ownSizeY = ownSize.GetY();
		SizeMode widthMode = GetWidthMode(), heightMode = GetHeightMode();

		place(MakeAxisX(_anchorMin, _anchorMax, _alignment, _offset, ownSize), parentSize.GetX(), center.GetX(), size.GetX(), currentSize.GetX(), widthMode, offsetX,
		      ownSizeX, widthMode);
		place(MakeAxisY(_anchorMin, _anchorMax, _alignment, _offset, ownSize), parentSize.GetY(), center.GetY(), size.GetY(), currentSize.GetY(), heightMode, offsetY,
		      ownSizeY, heightMode);

		SetWidthMode(widthMode);
		SetHeightMode(heightMode);
		SetSize(Vector2(ownSizeX, ownSizeY));
		SetOffset(Vector2(offsetX, offsetY));
	}

	const Vector2& AnchoredLayoutParams::GetAnchorMin() const
	{
		return _anchorMin;
	}

	void AnchoredLayoutParams::SetAnchorMin(const Vector2& anchorMin)
	{
		if (_anchorMin != anchorMin)
		{
			_anchorMin = anchorMin;
			MarkOwnerMeasureAsDirty();
			MarkOwnerArrangeAsDirty();
		}
	}

	const Vector2& AnchoredLayoutParams::GetAnchorMax() const
	{
		return _anchorMax;
	}

	void AnchoredLayoutParams::SetAnchorMax(const Vector2& anchorMax)
	{
		if (_anchorMax != anchorMax)
		{
			_anchorMax = anchorMax;
			MarkOwnerMeasureAsDirty();
			MarkOwnerArrangeAsDirty();
		}
	}

	const Vector2& AnchoredLayoutParams::GetAlignment() const
	{
		return _alignment;
	}

	void AnchoredLayoutParams::SetAlignment(const Vector2& alignment)
	{
		if (_alignment != alignment)
		{
			_alignment = alignment;
			MarkOwnerArrangeAsDirty();
		}
	}

	const Vector2& AnchoredLayoutParams::GetOffset() const
	{
		return _offset;
	}

	void AnchoredLayoutParams::SetOffset(const Vector2& offset)
	{
		if (_offset != offset)
		{
			_offset = offset;
			MarkOwnerArrangeAsDirty();
		}
	}
}

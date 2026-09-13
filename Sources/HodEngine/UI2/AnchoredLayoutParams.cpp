#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(AnchoredLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_anchorMin, "AnchorMin", &AnchoredLayoutParams::SetAnchorMin);
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_anchorMax, "AnchorMax", &AnchoredLayoutParams::SetAnchorMax);
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_pivot, "Pivot", &AnchoredLayoutParams::SetPivot);
		AddPropertyT(reflectionDescriptor, &AnchoredLayoutParams::_offset, "Offset", &AnchoredLayoutParams::SetOffset);
	}

	Vector2 AnchoredLayoutParams::ComputeSize(const Vector2& parentSize, const Vector2& desiredSize) const
	{
		Vector2 anchorSize = _anchorMax - _anchorMin;
		if (anchorSize != Vector2::Zero)
		{
			return (parentSize * anchorSize) + desiredSize;
		}

		return desiredSize;
	}

	Vector2 AnchoredLayoutParams::ComputePosition(const Vector2& parentSize, const Vector2& size) const
	{
		(void)size; // this LayoutParams type positions purely from anchors, not from the child's own resolved size

		const Vector2& anchorSize = _anchorMax - _anchorMin;
		const Vector2& anchorPos = (_anchorMin + _anchorMax) * 0.5f;

		Vector2 position = (anchorPos - Vector2(0.5f, 0.5f)) * parentSize;
		position += (_pivot - Vector2(0.5f, 0.5f)) * parentSize * anchorSize;
		position += _offset;

		return position;
	}

	bool AnchoredLayoutParams::DependsOnParentSize() const
	{
		return _anchorMin != _anchorMax;
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
			MarkOwnerSizeAsDirty();
			MarkOwnerLocalMatrixAsDirty();
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
			MarkOwnerSizeAsDirty();
			MarkOwnerLocalMatrixAsDirty();
		}
	}

	const Vector2& AnchoredLayoutParams::GetPivot() const
	{
		return _pivot;
	}

	void AnchoredLayoutParams::SetPivot(const Vector2& pivot)
	{
		if (_pivot != pivot)
		{
			_pivot = pivot;
			MarkOwnerLocalMatrixAsDirty();
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
			MarkOwnerLocalMatrixAsDirty();
		}
	}
}

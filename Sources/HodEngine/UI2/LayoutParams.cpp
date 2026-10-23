#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/Math.hpp>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(LayoutParams::SizeMode, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(LayoutParams::SizeMode::Fixed, "Fixed");
		reflectionDescriptor.AddEnumValue(LayoutParams::SizeMode::Content, "Content");
	}

	DESCRIBE_REFLECTED_CLASS(LayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &LayoutParams::_widthMode, "WidthMode", &LayoutParams::SetWidthMode);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_heightMode, "HeightMode", &LayoutParams::SetHeightMode);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_size, "Size", &LayoutParams::SetSize);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_minSize, "MinSize", &LayoutParams::SetMinSize);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_maxSize, "MaxSize", &LayoutParams::SetMaxSize);
	}

	LayoutParams::SizeMode LayoutParams::GetWidthMode() const
	{
		return _widthMode;
	}

	void LayoutParams::SetWidthMode(SizeMode widthMode)
	{
		if (_widthMode != widthMode)
		{
			_widthMode = widthMode;
			MarkOwnerMeasureAsDirty();
		}
	}

	LayoutParams::SizeMode LayoutParams::GetHeightMode() const
	{
		return _heightMode;
	}

	void LayoutParams::SetHeightMode(SizeMode heightMode)
	{
		if (_heightMode != heightMode)
		{
			_heightMode = heightMode;
			MarkOwnerMeasureAsDirty();
		}
	}

	const Vector2& LayoutParams::GetSize() const
	{
		return _size;
	}

	void LayoutParams::SetSize(const Vector2& size)
	{
		if (_size != size)
		{
			_size = size;
			MarkOwnerMeasureAsDirty();
		}
	}

	const Vector2& LayoutParams::GetMinSize() const
	{
		return _minSize;
	}

	void LayoutParams::SetMinSize(const Vector2& minSize)
	{
		if (_minSize != minSize)
		{
			_minSize = minSize;
			MarkOwnerMeasureAsDirty();
		}
	}

	const Vector2& LayoutParams::GetMaxSize() const
	{
		return _maxSize;
	}

	void LayoutParams::SetMaxSize(const Vector2& maxSize)
	{
		if (_maxSize != maxSize)
		{
			_maxSize = maxSize;
			MarkOwnerMeasureAsDirty();
		}
	}

	bool LayoutParams::DependsOnParentSize() const
	{
		return DependsOnParentWidth() || DependsOnParentHeight();
	}

	bool LayoutParams::DependsOnParentWidth() const
	{
		return false;
	}

	bool LayoutParams::DependsOnParentHeight() const
	{
		return false;
	}

	/// @brief
	/// @param contentSize
	/// @return
	Vector2 LayoutParams::ResolveSize(const Vector2& contentSize) const
	{
		float width = _widthMode == SizeMode::Fixed ? _size.GetX() : contentSize.GetX();
		float height = _heightMode == SizeMode::Fixed ? _size.GetY() : contentSize.GetY();
		return Vector2(Clamp(width, _minSize.GetX(), _maxSize.GetX()), Clamp(height, _minSize.GetY(), _maxSize.GetY()));
	}

	/// @brief
	/// @param availableSize
	/// @param contentSize
	/// @return
	Vector2 LayoutParams::ComputeSize(const Vector2& availableSize, const Vector2& contentSize) const
	{
		(void)availableSize;
		return ResolveSize(contentSize);
	}

	bool LayoutParams::TryComputePosition(const Rect& parentContentRect, const Vector2& size, Vector2& outPosition) const
	{
		(void)parentContentRect;
		(void)size;
		(void)outPosition;
		return false;
	}

	void LayoutParams::MarkOwnerMeasureAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkMeasureAsDirty();
		}
	}

	Node* LayoutParams::GetNode() const
	{
		return _node;
	}

	void LayoutParams::MarkOwnerArrangeAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkArrangeAsDirty();
		}
	}
}

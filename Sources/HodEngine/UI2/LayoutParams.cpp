#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(LayoutParams::HAlign, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(LayoutParams::HAlign::Left, "Left");
		reflectionDescriptor.AddEnumValue(LayoutParams::HAlign::Center, "Center");
		reflectionDescriptor.AddEnumValue(LayoutParams::HAlign::Right, "Right");
		reflectionDescriptor.AddEnumValue(LayoutParams::HAlign::Stretch, "Stretch");
	}

	DESCRIBE_REFLECTED_ENUM(LayoutParams::VAlign, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(LayoutParams::VAlign::Top, "Top");
		reflectionDescriptor.AddEnumValue(LayoutParams::VAlign::Center, "Center");
		reflectionDescriptor.AddEnumValue(LayoutParams::VAlign::Bottom, "Bottom");
		reflectionDescriptor.AddEnumValue(LayoutParams::VAlign::Stretch, "Stretch");
	}

	DESCRIBE_REFLECTED_CLASS(LayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &LayoutParams::_margin, "Margin", &LayoutParams::SetMargin);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_hAlign, "HAlign", &LayoutParams::SetHAlign);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_vAlign, "VAlign", &LayoutParams::SetVAlign);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_minSize, "MinSize", &LayoutParams::SetMinSize);
		AddPropertyT(reflectionDescriptor, &LayoutParams::_maxSize, "MaxSize", &LayoutParams::SetMaxSize);
	}

	const Margin& LayoutParams::GetMargin() const
	{
		return _margin;
	}

	void LayoutParams::SetMargin(const Margin& margin)
	{
		if (!(_margin == margin))
		{
			_margin = margin;
			MarkOwnerMeasureAsDirty();
		}
	}

	LayoutParams::HAlign LayoutParams::GetHAlign() const
	{
		return _hAlign;
	}

	void LayoutParams::SetHAlign(HAlign hAlign)
	{
		if (_hAlign != hAlign)
		{
			_hAlign = hAlign;
			MarkOwnerArrangeAsDirty();
		}
	}

	LayoutParams::VAlign LayoutParams::GetVAlign() const
	{
		return _vAlign;
	}

	void LayoutParams::SetVAlign(VAlign vAlign)
	{
		if (_vAlign != vAlign)
		{
			_vAlign = vAlign;
			MarkOwnerArrangeAsDirty();
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
		return false;
	}

	bool LayoutParams::TryComputeSize(const Vector2& availableSize, const Vector2& desiredSize, Vector2& outSize) const
	{
		(void)availableSize;
		(void)desiredSize;
		(void)outSize;
		return false;
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

	void LayoutParams::MarkOwnerArrangeAsDirty()
	{
		if (_node != nullptr)
		{
			_node->MarkArrangeAsDirty();
		}
	}
}

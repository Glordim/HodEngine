#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/BoxLayoutParams.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(BoxLayoutParams::Rule, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::Rule::OwnSize, "Own Size");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::Rule::Fill, "Fill");
	}

	DESCRIBE_REFLECTED_CLASS(BoxLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_margin, "Margin", &BoxLayoutParams::SetMargin);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_rule, "Rule", &BoxLayoutParams::SetRule);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_weight, "Weight", &BoxLayoutParams::SetWeight);
	}

	/// @brief The cross-axis alignment, that each concrete type names after its own axis (VerticalAlign in a row, HorizontalAlign in a column):
	/// kept by index, so a centered or stretched child stays so when its parent's layout turns from a row into a column.
	void BoxLayoutParams::CarryOverFrom(const LayoutParams& previous)
	{
		if (const BoxLayoutParams* previousBoxLayoutParams = LayoutParams::Cast<BoxLayoutParams>(&previous))
		{
			SetCrossAlignIndex(previousBoxLayoutParams->GetCrossAlignIndex());
		}
	}

	const Margin& BoxLayoutParams::GetMargin() const
	{
		return _margin;
	}

	void BoxLayoutParams::SetMargin(const Margin& margin)
	{
		if (!(_margin == margin))
		{
			_margin = margin;
			MarkOwnerMeasureAsDirty();
		}
	}

	BoxLayoutParams::Rule BoxLayoutParams::GetRule() const
	{
		return _rule;
	}

	void BoxLayoutParams::SetRule(Rule rule)
	{
		if (_rule != rule)
		{
			_rule = rule;
			MarkOwnerMeasureAsDirty();
		}
	}

	float BoxLayoutParams::GetWeight() const
	{
		return _weight;
	}

	void BoxLayoutParams::SetWeight(float weight)
	{
		if (_weight != weight)
		{
			_weight = weight;
			MarkOwnerArrangeAsDirty();
		}
	}

	DESCRIBE_REFLECTED_ENUM(HorizontalLayoutParams::VerticalAlign, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(HorizontalLayoutParams::VerticalAlign::Top, "Top");
		reflectionDescriptor.AddEnumValue(HorizontalLayoutParams::VerticalAlign::Center, "Center");
		reflectionDescriptor.AddEnumValue(HorizontalLayoutParams::VerticalAlign::Bottom, "Bottom");
		reflectionDescriptor.AddEnumValue(HorizontalLayoutParams::VerticalAlign::Stretch, "Stretch");
	}

	DESCRIBE_REFLECTED_CLASS(HorizontalLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &HorizontalLayoutParams::_verticalAlign, "VerticalAlign", &HorizontalLayoutParams::SetVerticalAlign);
	}

	HorizontalLayoutParams::VerticalAlign HorizontalLayoutParams::GetVerticalAlign() const
	{
		return _verticalAlign;
	}

	void HorizontalLayoutParams::SetVerticalAlign(VerticalAlign verticalAlign)
	{
		if (_verticalAlign != verticalAlign)
		{
			_verticalAlign = verticalAlign;
			MarkOwnerArrangeAsDirty();
		}
	}

	uint8_t HorizontalLayoutParams::GetCrossAlignIndex() const
	{
		return static_cast<uint8_t>(_verticalAlign);
	}

	void HorizontalLayoutParams::SetCrossAlignIndex(uint8_t crossAlignIndex)
	{
		SetVerticalAlign(static_cast<VerticalAlign>(crossAlignIndex));
	}

	DESCRIBE_REFLECTED_ENUM(VerticalLayoutParams::HorizontalAlign, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(VerticalLayoutParams::HorizontalAlign::Left, "Left");
		reflectionDescriptor.AddEnumValue(VerticalLayoutParams::HorizontalAlign::Center, "Center");
		reflectionDescriptor.AddEnumValue(VerticalLayoutParams::HorizontalAlign::Right, "Right");
		reflectionDescriptor.AddEnumValue(VerticalLayoutParams::HorizontalAlign::Stretch, "Stretch");
	}

	DESCRIBE_REFLECTED_CLASS(VerticalLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &VerticalLayoutParams::_horizontalAlign, "HorizontalAlign", &VerticalLayoutParams::SetHorizontalAlign);
	}

	VerticalLayoutParams::HorizontalAlign VerticalLayoutParams::GetHorizontalAlign() const
	{
		return _horizontalAlign;
	}

	void VerticalLayoutParams::SetHorizontalAlign(HorizontalAlign horizontalAlign)
	{
		if (_horizontalAlign != horizontalAlign)
		{
			_horizontalAlign = horizontalAlign;
			MarkOwnerArrangeAsDirty();
		}
	}

	uint8_t VerticalLayoutParams::GetCrossAlignIndex() const
	{
		return static_cast<uint8_t>(_horizontalAlign);
	}

	void VerticalLayoutParams::SetCrossAlignIndex(uint8_t crossAlignIndex)
	{
		SetHorizontalAlign(static_cast<HorizontalAlign>(crossAlignIndex));
	}
}

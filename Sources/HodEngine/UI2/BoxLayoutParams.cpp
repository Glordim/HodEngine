#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/BoxLayoutParams.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(BoxLayoutParams::Rule, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::Rule::Auto, "Auto");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::Rule::Fill, "Fill");
	}

	DESCRIBE_REFLECTED_ENUM(BoxLayoutParams::HAlign, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::HAlign::Left, "Left");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::HAlign::Center, "Center");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::HAlign::Right, "Right");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::HAlign::Stretch, "Stretch");
	}

	DESCRIBE_REFLECTED_ENUM(BoxLayoutParams::VAlign, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::VAlign::Top, "Top");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::VAlign::Center, "Center");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::VAlign::Bottom, "Bottom");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::VAlign::Stretch, "Stretch");
	}

	DESCRIBE_REFLECTED_CLASS(BoxLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_margin, "Margin", &BoxLayoutParams::SetMargin);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_hAlign, "HAlign", &BoxLayoutParams::SetHAlign);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_vAlign, "VAlign", &BoxLayoutParams::SetVAlign);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_rule, "Rule", &BoxLayoutParams::SetRule);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_weight, "Weight", &BoxLayoutParams::SetWeight);
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

	BoxLayoutParams::HAlign BoxLayoutParams::GetHAlign() const
	{
		return _hAlign;
	}

	void BoxLayoutParams::SetHAlign(HAlign hAlign)
	{
		if (_hAlign != hAlign)
		{
			_hAlign = hAlign;
			MarkOwnerArrangeAsDirty();
		}
	}

	BoxLayoutParams::VAlign BoxLayoutParams::GetVAlign() const
	{
		return _vAlign;
	}

	void BoxLayoutParams::SetVAlign(VAlign vAlign)
	{
		if (_vAlign != vAlign)
		{
			_vAlign = vAlign;
			MarkOwnerArrangeAsDirty();
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
}

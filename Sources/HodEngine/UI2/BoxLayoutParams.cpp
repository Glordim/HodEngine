#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/BoxLayoutParams.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(BoxLayoutParams::Rule, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::Rule::Auto, "Auto");
		reflectionDescriptor.AddEnumValue(BoxLayoutParams::Rule::Fill, "Fill");
	}

	DESCRIBE_REFLECTED_CLASS(BoxLayoutParams, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_rule, "Rule", &BoxLayoutParams::SetRule);
		AddPropertyT(reflectionDescriptor, &BoxLayoutParams::_weight, "Weight", &BoxLayoutParams::SetWeight);
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

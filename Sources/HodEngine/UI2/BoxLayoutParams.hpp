#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"

namespace hod::inline ui2
{
	/// @brief Per-child layout parameters for BoxLayout. Along the container's main axis, an
	/// Auto child gets exactly its own measured extent; a Fill child shares whatever space is left
	/// over after every Auto child, proportionally to its weight. Cross-axis placement uses the
	/// common HAlign/VAlign/margin/min-max fields inherited from LayoutParams.
	class HOD_UI2_API BoxLayoutParams : public LayoutParams
	{
		REFLECTED_CLASS(BoxLayoutParams, LayoutParams)

	public:

		enum class Rule : uint8_t { Auto, Fill };
		REFLECTED_ENUM(HOD_UI2_API, Rule);

	public:

		Rule						GetRule() const;
		void						SetRule(Rule rule);

		float						GetWeight() const;
		void						SetWeight(float weight);

	private:

		Rule						_rule = Rule::Auto;
		float						_weight = 1.0f;
	};
}

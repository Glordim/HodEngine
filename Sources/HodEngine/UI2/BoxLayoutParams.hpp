#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/Margin.hpp"

namespace hod::inline ui2
{
	/// @brief Per-child layout parameters for BoxLayout. Along the container's main axis, an
	/// Auto child gets exactly its own size (LayoutParams' SizeMode/Size); a Fill child shares whatever space is left
	/// over after every Auto child, proportionally to its weight. Cross-axis placement uses HAlign/VAlign
	/// and the min-max size inherited from LayoutParams.
	/// The margin is the space kept around the child, in the flow: it pushes its neighbours away.
	class HOD_UI2_API BoxLayoutParams : public LayoutParams
	{
		REFLECTED_CLASS(BoxLayoutParams, LayoutParams)

	public:

		enum class Rule : uint8_t { Auto, Fill };
		REFLECTED_ENUM(HOD_UI2_API, Rule);

		// Stretch is index 3 in both, as Node::ComputeAlignedExtent expects
		enum class HAlign : uint8_t { Left, Center, Right, Stretch };
		REFLECTED_ENUM(HOD_UI2_API, HAlign);

		enum class VAlign : uint8_t { Top, Center, Bottom, Stretch };
		REFLECTED_ENUM(HOD_UI2_API, VAlign);

	public:

		const Margin&				GetMargin() const;
		void						SetMargin(const Margin& margin);

		HAlign						GetHAlign() const;
		void						SetHAlign(HAlign hAlign);

		VAlign						GetVAlign() const;
		void						SetVAlign(VAlign vAlign);

		Rule						GetRule() const;
		void						SetRule(Rule rule);

		float						GetWeight() const;
		void						SetWeight(float weight);

	private:

		Margin						_margin;
		HAlign						_hAlign = HAlign::Stretch;
		VAlign						_vAlign = VAlign::Stretch;
		Rule						_rule = Rule::Auto;
		float						_weight = 1.0f;
	};
}

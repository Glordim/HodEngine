#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/Margin.hpp"

namespace hod::inline ui2
{
	/// @brief Base of the per-child layout parameters of a BoxLayout (see HorizontalLayoutParams, VerticalLayoutParams).
	/// Along the container's main axis, an Own Size child gets exactly its own size (LayoutParams' SizeMode/Size); a Fill child
	/// shares whatever space is left over after every Own Size child, proportionally to its weight. Cross-axis placement uses
	/// the alignment the concrete type adds and the min-max size inherited from LayoutParams.
	/// The margin is the space kept around the child, in the flow: it pushes its neighbours away.
	class HOD_UI2_API BoxLayoutParams : public LayoutParams
	{
		REFLECTED_CLASS(BoxLayoutParams, LayoutParams)

	public:

		enum class Rule : uint8_t { OwnSize, Fill };
		REFLECTED_ENUM(HOD_UI2_API, Rule);

	public:

		void						CarryOverFrom(const LayoutParams& previous) override;

		const Margin&				GetMargin() const;
		void						SetMargin(const Margin& margin);

		Rule						GetRule() const;
		void						SetRule(Rule rule);

		float						GetWeight() const;
		void						SetWeight(float weight);

		// Placement on the cross axis, whatever the axis, as Node::ComputeAlignedExtent expects it:
		// 0 top / left, 1 centered, 2 bottom / right, 3 stretched
		virtual uint8_t				GetCrossAlignIndex() const = 0;
		virtual void				SetCrossAlignIndex(uint8_t crossAlignIndex) = 0;

	private:

		Margin						_margin;
		Rule						_rule = Rule::OwnSize;
		float						_weight = 1.0f;
	};

	/// @brief Layout parameters of a HorizontalLayout's child: its vertical placement within the row's height.
	class HOD_UI2_API HorizontalLayoutParams : public BoxLayoutParams
	{
		REFLECTED_CLASS(HorizontalLayoutParams, BoxLayoutParams)

	public:

		// Stretch is index 3, as Node::ComputeAlignedExtent expects
		enum class VerticalAlign : uint8_t { Top, Center, Bottom, Stretch };
		REFLECTED_ENUM(HOD_UI2_API, VerticalAlign);

	public:

		VerticalAlign				GetVerticalAlign() const;
		void						SetVerticalAlign(VerticalAlign verticalAlign);

		uint8_t						GetCrossAlignIndex() const override;
		void						SetCrossAlignIndex(uint8_t crossAlignIndex) override;

	private:

		VerticalAlign				_verticalAlign = VerticalAlign::Stretch;
	};

	/// @brief Layout parameters of a VerticalLayout's child: its horizontal placement within the column's width.
	class HOD_UI2_API VerticalLayoutParams : public BoxLayoutParams
	{
		REFLECTED_CLASS(VerticalLayoutParams, BoxLayoutParams)

	public:

		// Stretch is index 3, as Node::ComputeAlignedExtent expects
		enum class HorizontalAlign : uint8_t { Left, Center, Right, Stretch };
		REFLECTED_ENUM(HOD_UI2_API, HorizontalAlign);

	public:

		HorizontalAlign				GetHorizontalAlign() const;
		void						SetHorizontalAlign(HorizontalAlign horizontalAlign);

		uint8_t						GetCrossAlignIndex() const override;
		void						SetCrossAlignIndex(uint8_t crossAlignIndex) override;

	private:

		HorizontalAlign				_horizontalAlign = HorizontalAlign::Stretch;
	};
}

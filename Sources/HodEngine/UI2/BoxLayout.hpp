#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Layout.hpp"
#include "HodEngine/UI2/Padding.hpp"

namespace hod::inline ui2
{
	/// @brief Base of the layouts laying a node's children out one after the other along one axis (see HorizontalLayout,
	/// VerticalLayout): a single algorithm indexed by axis rather than two separate implementations. Each child's
	/// BoxLayoutParams::Rule picks whether it takes exactly its own measured extent along the main axis (Own Size) or shares
	/// whatever space is left over, proportionally to its weight (Fill); its cross-axis placement uses its alignment
	/// (HorizontalLayoutParams' VerticalAlign, VerticalLayoutParams' HorizontalAlign), its margin and the min-max size every LayoutParams has.
	/// When no child fills the main axis, the children as a group are put on it by the layout's own alignment.
	/// The padding insets the owner's content rect before any of it: the space kept inside the owner, around all its
	/// children (where a child's margin is around that child only).
	/// The flow goes left to right (HorizontalLayout) or top to bottom (VerticalLayout).
	class HOD_UI2_API BoxLayout : public Layout
	{
		REFLECTED_CLASS(BoxLayout, Layout)

	public:

		enum class Axis : uint8_t { Horizontal, Vertical };

	public:

		Vector2						Measure(Node& owner, const Vector2& availableSize) override;
		void						Arrange(Node& owner, const Rect& contentRect) override;

		void						CarryOverFrom(const Layout& previous) override;

		Axis						GetAxis() const;

		float						GetSpacing() const;
		void						SetSpacing(float spacing);

		const Padding&				GetPadding() const;
		void						SetPadding(const Padding& padding);

		// Where the children, as a group, go along the main axis when none of them fills it, whatever the axis:
		// 0 at the flow's start (left / top), 1 centered, 2 at its end (right / bottom), or spread over it, the free space added to the spacing:
		// 3 between the children only (none on the edges), 4 around each child (half a gap on the edges), 5 evenly (a whole gap on the edges)
		virtual uint8_t				GetMainAlignIndex() const = 0;
		virtual void				SetMainAlignIndex(uint8_t mainAlignIndex) = 0;

	protected:

		explicit					BoxLayout(Axis axis);

	private:

		Axis						_axis;
		float						_spacing = 0.0f;
		Padding						_padding;
	};

	/// @brief Lays the children out in a row, left to right (see BoxLayout). Its children get HorizontalLayoutParams.
	class HOD_UI2_API HorizontalLayout : public BoxLayout
	{
		REFLECTED_CLASS(HorizontalLayout, BoxLayout)

	public:

		enum class Alignment : uint8_t { Left, Center, Right, SpaceBetween, SpaceAround, SpaceEvenly };
		REFLECTED_ENUM(HOD_UI2_API, Alignment);

	public:

									HorizontalLayout();

		LayoutParams*				CreateDefaultLayoutParams() const override;

		// Where the children go when they don't fill the width: together (Left, Center, Right), or spread (see BoxLayout::GetMainAlignIndex)
		Alignment					GetAlignment() const;
		void						SetAlignment(Alignment alignment);

		uint8_t						GetMainAlignIndex() const override;
		void						SetMainAlignIndex(uint8_t mainAlignIndex) override;

	private:

		Alignment					_alignment = Alignment::Left;
	};

	/// @brief Lays the children out in a column, top to bottom (see BoxLayout). Its children get VerticalLayoutParams.
	class HOD_UI2_API VerticalLayout : public BoxLayout
	{
		REFLECTED_CLASS(VerticalLayout, BoxLayout)

	public:

		enum class Alignment : uint8_t { Top, Center, Bottom, SpaceBetween, SpaceAround, SpaceEvenly };
		REFLECTED_ENUM(HOD_UI2_API, Alignment);

	public:

									VerticalLayout();

		LayoutParams*				CreateDefaultLayoutParams() const override;

		// Where the children go when they don't fill the height: together (Top, Center, Bottom), or spread (see BoxLayout::GetMainAlignIndex)
		Alignment					GetAlignment() const;
		void						SetAlignment(Alignment alignment);

		uint8_t						GetMainAlignIndex() const override;
		void						SetMainAlignIndex(uint8_t mainAlignIndex) override;

	private:

		Alignment					_alignment = Alignment::Top;
	};
}

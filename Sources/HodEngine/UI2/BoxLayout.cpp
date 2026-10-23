#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/BoxLayout.hpp"
#include "HodEngine/UI2/BoxLayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <algorithm>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(BoxLayout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &BoxLayout::_padding, "Padding", &BoxLayout::SetPadding);
		AddPropertyT(reflectionDescriptor, &BoxLayout::_spacing, "Spacing", &BoxLayout::SetSpacing);
	}

	BoxLayout::BoxLayout(Axis axis)
	: _axis(axis)
	{
	}

	/// @brief
	/// @param owner
	/// @param availableSize
	/// @return
	Vector2 BoxLayout::Measure(Node& owner, const Vector2& availableSize)
	{
		bool isHorizontal = (_axis == Axis::Horizontal);

		Vector2 paddingSize(_padding._left + _padding._right, _padding._top + _padding._bottom);
		float   mainAvailable = std::max((isHorizontal ? availableSize.GetX() : availableSize.GetY()) - (isHorizontal ? paddingSize.GetX() : paddingSize.GetY()), 0.0f);
		float   crossAvailable = std::max((isHorizontal ? availableSize.GetY() : availableSize.GetX()) - (isHorizontal ? paddingSize.GetY() : paddingSize.GetX()), 0.0f);

		float mainTotal = 0.0f;
		float crossMax = 0.0f;
		bool  first = true;

		for (Node* child : owner.GetChildren())
		{
			if (child == nullptr)
			{
				continue;
			}

			LayoutParams*    childLayoutParams = child->GetLayoutParams();
			BoxLayoutParams* boxLayoutParams = LayoutParams::Cast<BoxLayoutParams>(childLayoutParams);
			bool             isFill = (boxLayoutParams != nullptr && boxLayoutParams->GetRule() == BoxLayoutParams::Rule::Fill);

			Margin margin = boxLayoutParams != nullptr ? boxLayoutParams->GetMargin() : Margin();
			float  marginMain = isHorizontal ? (margin._left + margin._right) : (margin._top + margin._bottom);
			float  marginCross = isHorizontal ? (margin._top + margin._bottom) : (margin._left + margin._right);

			// Fill children are measured with zero main-axis space: their real share of the leftover
			// space is only known once Arrange sums up every Own Size child's actual measured extent.
			float mainForChild = isFill ? 0.0f : std::max(mainAvailable - marginMain, 0.0f);
			float crossForChild = std::max(crossAvailable - marginCross, 0.0f);

			Vector2 childAvailable = isHorizontal ? Vector2(mainForChild, crossForChild) : Vector2(crossForChild, mainForChild);
			Vector2 childMeasured = child->Measure(childAvailable);

			float childMain = (isHorizontal ? childMeasured.GetX() : childMeasured.GetY()) + marginMain;
			float childCross = (isHorizontal ? childMeasured.GetY() : childMeasured.GetX()) + marginCross;

			if (isFill == false)
			{
				mainTotal += childMain;
			}
			crossMax = std::max(crossMax, childCross);

			if (first == false)
			{
				mainTotal += _spacing;
			}
			first = false;
		}

		return (isHorizontal ? Vector2(mainTotal, crossMax) : Vector2(crossMax, mainTotal)) + paddingSize;
	}

	/// @brief The main axis is walked in flow order (left to right, top to bottom): a position along it is first
	/// a distance from the flow's start, turned into a coordinate at the end (Y-up, so a column starts from its top).
	/// @param owner
	/// @param ownerContentRect
	void BoxLayout::Arrange(Node& owner, const Rect& ownerContentRect)
	{
		bool isHorizontal = (_axis == Axis::Horizontal);

		Rect contentRect = ownerContentRect.Inset(_padding);
		contentRect._size = Vector2(std::max(contentRect._size.GetX(), 0.0f), std::max(contentRect._size.GetY(), 0.0f));

		float mainStart = isHorizontal ? contentRect._position.GetX() : contentRect._position.GetY();
		float mainAvailable = isHorizontal ? contentRect._size.GetX() : contentRect._size.GetY();
		float crossStart = isHorizontal ? contentRect._position.GetY() : contentRect._position.GetX();
		float crossAvailable = isHorizontal ? contentRect._size.GetY() : contentRect._size.GetX();

		const Vector<Node*>& children = owner.GetChildren();

		float    autoMainTotal = 0.0f;
		float    fillWeightTotal = 0.0f;
		uint32_t childCount = 0;

		for (Node* child : children)
		{
			if (child == nullptr)
			{
				continue;
			}
			++childCount;

			LayoutParams*    childLayoutParams = child->GetLayoutParams();
			BoxLayoutParams* boxLayoutParams = LayoutParams::Cast<BoxLayoutParams>(childLayoutParams);
			bool             isFill = (boxLayoutParams != nullptr && boxLayoutParams->GetRule() == BoxLayoutParams::Rule::Fill);

			if (isFill)
			{
				fillWeightTotal += boxLayoutParams->GetWeight();
			}
			else
			{
				Margin margin = boxLayoutParams != nullptr ? boxLayoutParams->GetMargin() : Margin();
				float  marginMain = isHorizontal ? (margin._left + margin._right) : (margin._top + margin._bottom);
				autoMainTotal += (isHorizontal ? child->GetMeasuredSize().GetX() : child->GetMeasuredSize().GetY()) + marginMain;
			}
		}

		float spacingTotal = childCount > 0 ? _spacing * (float)(childCount - 1) : 0.0f;
		float remainingForFill = std::max(mainAvailable - autoMainTotal - spacingTotal, 0.0f);

		// What no child takes (none fills), split around the children as a group by the layout's alignment
		float   leftover = fillWeightTotal > 0.0f ? 0.0f : remainingForFill;
		uint8_t mainAlignIndex = GetMainAlignIndex();
		float   flowCursor = mainAlignIndex == 1 ? leftover * 0.5f : (mainAlignIndex == 2 ? leftover : 0.0f);
		bool    first = true;

		for (Node* child : children)
		{
			if (child == nullptr)
			{
				continue;
			}

			if (first == false)
			{
				flowCursor += _spacing;
			}
			first = false;

			LayoutParams*    childLayoutParams = child->GetLayoutParams();
			BoxLayoutParams* boxLayoutParams = LayoutParams::Cast<BoxLayoutParams>(childLayoutParams);
			bool             isFill = (boxLayoutParams != nullptr && boxLayoutParams->GetRule() == BoxLayoutParams::Rule::Fill);

			// In flow order: the start is the left in a row, the top in a column
			Margin margin = boxLayoutParams != nullptr ? boxLayoutParams->GetMargin() : Margin();
			float  marginFlowStart = isHorizontal ? margin._left : margin._top;
			float  marginFlowEnd = isHorizontal ? margin._right : margin._bottom;
			float  marginCrossStart = isHorizontal ? margin._bottom : margin._left;
			float  marginCrossEnd = isHorizontal ? margin._top : margin._right;

			float childMainExtent;
			if (isFill)
			{
				float weight = boxLayoutParams->GetWeight();
				float share = (fillWeightTotal > 0.0f) ? (remainingForFill * (weight / fillWeightTotal)) : 0.0f;
				childMainExtent = std::max(share - marginFlowStart - marginFlowEnd, 0.0f);
			}
			else
			{
				childMainExtent = isHorizontal ? child->GetMeasuredSize().GetX() : child->GetMeasuredSize().GetY();
			}

			float childFlowOffset = flowCursor + marginFlowStart;
			flowCursor = childFlowOffset + childMainExtent + marginFlowEnd;
			float childMainOffset = isHorizontal ? mainStart + childFlowOffset : mainStart + mainAvailable - childFlowOffset - childMainExtent;

			Vector2 minSize = childLayoutParams != nullptr ? childLayoutParams->GetMinSize() : Vector2::Zero;
			Vector2 maxSize = childLayoutParams != nullptr ? childLayoutParams->GetMaxSize() : Vector2::Infinity;
			uint8_t crossAlign = boxLayoutParams != nullptr ? boxLayoutParams->GetCrossAlignIndex() : 3; // Stretched without BoxLayoutParams
			float   crossDesired = isHorizontal ? child->GetMeasuredSize().GetY() : child->GetMeasuredSize().GetX();
			float   crossMinExtent = isHorizontal ? minSize.GetY() : minSize.GetX();
			float   crossMaxExtent = isHorizontal ? maxSize.GetY() : maxSize.GetX();

			float crossOffset, crossExtent;
			Node::ComputeAlignedExtent(
				crossStart + marginCrossStart, std::max(crossAvailable - marginCrossStart - marginCrossEnd, 0.0f), crossDesired, crossMinExtent, crossMaxExtent, crossAlign, isHorizontal == false,
				crossOffset, crossExtent);

			Rect childFinalRect;
			childFinalRect._position = isHorizontal ? Vector2(childMainOffset, crossOffset) : Vector2(crossOffset, childMainOffset);
			childFinalRect._size = isHorizontal ? Vector2(childMainExtent, crossExtent) : Vector2(crossExtent, childMainExtent);

			child->Arrange(childFinalRect);
		}
	}

	/// @brief The alignment of the children as a group, that each concrete type names after its own axis: kept by index,
	/// so a centered row stays a centered column.
	void BoxLayout::CarryOverFrom(const Layout& previous)
	{
		if (const BoxLayout* previousBoxLayout = Layout::Cast<BoxLayout>(&previous))
		{
			SetMainAlignIndex(previousBoxLayout->GetMainAlignIndex());
		}
	}

	BoxLayout::Axis BoxLayout::GetAxis() const
	{
		return _axis;
	}

	float BoxLayout::GetSpacing() const
	{
		return _spacing;
	}

	void BoxLayout::SetSpacing(float spacing)
	{
		if (_spacing != spacing)
		{
			_spacing = spacing;
			MarkOwnerMeasureAsDirty();
		}
	}

	const Padding& BoxLayout::GetPadding() const
	{
		return _padding;
	}

	void BoxLayout::SetPadding(const Padding& padding)
	{
		if (!(_padding == padding))
		{
			_padding = padding;
			MarkOwnerMeasureAsDirty();
		}
	}

	DESCRIBE_REFLECTED_ENUM(HorizontalLayout::Alignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(HorizontalLayout::Alignment::Left, "Left");
		reflectionDescriptor.AddEnumValue(HorizontalLayout::Alignment::Center, "Center");
		reflectionDescriptor.AddEnumValue(HorizontalLayout::Alignment::Right, "Right");
	}

	DESCRIBE_REFLECTED_CLASS(HorizontalLayout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &HorizontalLayout::_alignment, "Alignment", &HorizontalLayout::SetAlignment);
	}

	HorizontalLayout::HorizontalLayout()
	: BoxLayout(Axis::Horizontal)
	{
	}

	LayoutParams* HorizontalLayout::CreateDefaultLayoutParams() const
	{
		return DefaultAllocator::GetInstance().New<HorizontalLayoutParams>();
	}

	HorizontalLayout::Alignment HorizontalLayout::GetAlignment() const
	{
		return _alignment;
	}

	void HorizontalLayout::SetAlignment(Alignment alignment)
	{
		if (_alignment != alignment)
		{
			_alignment = alignment;
			MarkOwnerArrangeAsDirty();
		}
	}

	uint8_t HorizontalLayout::GetMainAlignIndex() const
	{
		return static_cast<uint8_t>(_alignment);
	}

	void HorizontalLayout::SetMainAlignIndex(uint8_t mainAlignIndex)
	{
		SetAlignment(static_cast<Alignment>(mainAlignIndex));
	}

	DESCRIBE_REFLECTED_ENUM(VerticalLayout::Alignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(VerticalLayout::Alignment::Top, "Top");
		reflectionDescriptor.AddEnumValue(VerticalLayout::Alignment::Center, "Center");
		reflectionDescriptor.AddEnumValue(VerticalLayout::Alignment::Bottom, "Bottom");
	}

	DESCRIBE_REFLECTED_CLASS(VerticalLayout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &VerticalLayout::_alignment, "Alignment", &VerticalLayout::SetAlignment);
	}

	VerticalLayout::VerticalLayout()
	: BoxLayout(Axis::Vertical)
	{
	}

	LayoutParams* VerticalLayout::CreateDefaultLayoutParams() const
	{
		return DefaultAllocator::GetInstance().New<VerticalLayoutParams>();
	}

	VerticalLayout::Alignment VerticalLayout::GetAlignment() const
	{
		return _alignment;
	}

	void VerticalLayout::SetAlignment(Alignment alignment)
	{
		if (_alignment != alignment)
		{
			_alignment = alignment;
			MarkOwnerArrangeAsDirty();
		}
	}

	uint8_t VerticalLayout::GetMainAlignIndex() const
	{
		return static_cast<uint8_t>(_alignment);
	}

	void VerticalLayout::SetMainAlignIndex(uint8_t mainAlignIndex)
	{
		SetAlignment(static_cast<Alignment>(mainAlignIndex));
	}
}

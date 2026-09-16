#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/BoxLayout.hpp"
#include "HodEngine/UI2/BoxLayoutParams.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <algorithm>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_ENUM(BoxLayout::Axis, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(BoxLayout::Axis::Horizontal, "Horizontal");
		reflectionDescriptor.AddEnumValue(BoxLayout::Axis::Vertical, "Vertical");
	}

	DESCRIBE_REFLECTED_CLASS(BoxLayout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &BoxLayout::_axis, "Axis", &BoxLayout::SetAxis);
		AddPropertyT(reflectionDescriptor, &BoxLayout::_spacing, "Spacing", &BoxLayout::SetSpacing);
	}

	/// @brief
	/// @param owner
	/// @param availableSize
	/// @return
	Vector2 BoxLayout::Measure(Node& owner, const Vector2& availableSize)
	{
		bool isHorizontal = (_axis == Axis::Horizontal);

		float mainAvailable = isHorizontal ? availableSize.GetX() : availableSize.GetY();
		float crossAvailable = isHorizontal ? availableSize.GetY() : availableSize.GetX();

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

			Margin margin = childLayoutParams != nullptr ? childLayoutParams->GetMargin() : Margin();
			float  marginMain = isHorizontal ? (margin._left + margin._right) : (margin._top + margin._bottom);
			float  marginCross = isHorizontal ? (margin._top + margin._bottom) : (margin._left + margin._right);

			// Fill children are measured with zero main-axis space: their real share of the leftover
			// space is only known once Arrange sums up every Auto child's actual measured extent.
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

		return isHorizontal ? Vector2(mainTotal, crossMax) : Vector2(crossMax, mainTotal);
	}

	/// @brief
	/// @param owner
	/// @param contentRect
	void BoxLayout::Arrange(Node& owner, const Rect& contentRect)
	{
		bool isHorizontal = (_axis == Axis::Horizontal);

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
				Margin margin = childLayoutParams != nullptr ? childLayoutParams->GetMargin() : Margin();
				float  marginMain = isHorizontal ? (margin._left + margin._right) : (margin._top + margin._bottom);
				autoMainTotal += (isHorizontal ? child->GetMeasuredSize().GetX() : child->GetMeasuredSize().GetY()) + marginMain;
			}
		}

		float spacingTotal = childCount > 0 ? _spacing * (float)(childCount - 1) : 0.0f;
		float remainingForFill = std::max(mainAvailable - autoMainTotal - spacingTotal, 0.0f);

		float cursor = mainStart;
		bool  first = true;

		for (Node* child : children)
		{
			if (child == nullptr)
			{
				continue;
			}

			if (first == false)
			{
				cursor += _spacing;
			}
			first = false;

			LayoutParams*    childLayoutParams = child->GetLayoutParams();
			BoxLayoutParams* boxLayoutParams = LayoutParams::Cast<BoxLayoutParams>(childLayoutParams);
			bool             isFill = (boxLayoutParams != nullptr && boxLayoutParams->GetRule() == BoxLayoutParams::Rule::Fill);

			Margin margin = childLayoutParams != nullptr ? childLayoutParams->GetMargin() : Margin();
			float  marginMainStart = isHorizontal ? margin._left : margin._bottom;
			float  marginMainEnd = isHorizontal ? margin._right : margin._top;
			float  marginCrossStart = isHorizontal ? margin._bottom : margin._left;
			float  marginCrossEnd = isHorizontal ? margin._top : margin._right;

			float childMainExtent;
			if (isFill)
			{
				float weight = boxLayoutParams->GetWeight();
				float share = (fillWeightTotal > 0.0f) ? (remainingForFill * (weight / fillWeightTotal)) : 0.0f;
				childMainExtent = std::max(share - marginMainStart - marginMainEnd, 0.0f);
			}
			else
			{
				childMainExtent = isHorizontal ? child->GetMeasuredSize().GetX() : child->GetMeasuredSize().GetY();
			}

			float childMainOffset = cursor + marginMainStart;
			cursor = childMainOffset + childMainExtent + marginMainEnd;

			Vector2 minSize = childLayoutParams != nullptr ? childLayoutParams->GetMinSize() : Vector2::Zero;
			Vector2 maxSize = childLayoutParams != nullptr ? childLayoutParams->GetMaxSize() : Vector2::Infinity;
			uint8_t crossAlign = isHorizontal ? (uint8_t)(childLayoutParams != nullptr ? childLayoutParams->GetVAlign() : LayoutParams::VAlign::Stretch)
			                                   : (uint8_t)(childLayoutParams != nullptr ? childLayoutParams->GetHAlign() : LayoutParams::HAlign::Stretch);
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

	LayoutParams* BoxLayout::CreateDefaultLayoutParams() const
	{
		return DefaultAllocator::GetInstance().New<BoxLayoutParams>();
	}

	BoxLayout::Axis BoxLayout::GetAxis() const
	{
		return _axis;
	}

	void BoxLayout::SetAxis(Axis axis)
	{
		if (_axis != axis)
		{
			_axis = axis;
			MarkOwnerMeasureAsDirty();
		}
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
}

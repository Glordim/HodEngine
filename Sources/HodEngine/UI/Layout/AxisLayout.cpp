#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout/AxisLayout.hpp"

#undef min
#undef max

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(AxisLayout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &AxisLayout::_controlWidth, "_controlWidth", &AxisLayout::SetControlWidth);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_controlHeight, "_controlHeight", &AxisLayout::SetControlHeight);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_forceExpandWidth, "_forceExpandWidth", &AxisLayout::SetForceExpandWidth);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_forceExpandHeight, "_forceExpandHeight", &AxisLayout::SetForceExpandHeight);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_considerScaleWidth, "_considerScaleWidth", &AxisLayout::SetConsiderScaleWidth);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_considerScaleHeight, "_considerScaleHeight", &AxisLayout::SetConsiderScaleHeight);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_spacing, "_spacing", &AxisLayout::SetSpacing);
		AddPropertyT(reflectionDescriptor, &AxisLayout::_reverseArrangement, "_reverseArrangement", &AxisLayout::SetReverseArrangement);
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetControlWidth() const
	{
		return _controlWidth;
	}

	/// @brief 
	/// @param controlWidth 
	void AxisLayout::SetControlWidth(bool controlWidth)
	{
		if (_controlWidth != controlWidth)
		{
			_controlWidth = controlWidth;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetControlHeight() const
	{
		return _controlHeight;
	}

	/// @brief 
	/// @param controlHeight 
	void AxisLayout::SetControlHeight(bool controlHeight)
	{
		if (_controlHeight != controlHeight)
		{
			_controlHeight = controlHeight;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetForceExpandWidth() const
	{
		return _forceExpandWidth;
	}

	/// @brief 
	/// @param forceExpandWidth 
	void AxisLayout::SetForceExpandWidth(bool forceExpandWidth)
	{
		if (_forceExpandWidth != forceExpandWidth)
		{
			_forceExpandWidth = forceExpandWidth;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetForceExpandHeight() const
	{
		return _forceExpandHeight;
	}

	/// @brief 
	/// @param forceExpandHeight 
	void AxisLayout::SetForceExpandHeight(bool forceExpandHeight)
	{
		if (_forceExpandHeight != forceExpandHeight)
		{
			_forceExpandHeight = forceExpandHeight;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetConsiderScaleWidth() const
	{
		return _considerScaleWidth;
	}

	/// @brief 
	/// @param bScaleWidth 
	void AxisLayout::SetConsiderScaleWidth(bool considerScaleWidth)
	{
		if (_considerScaleWidth != considerScaleWidth)
		{
			_considerScaleWidth = considerScaleWidth;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetConsiderScaleHeight() const
	{
		return _considerScaleHeight;
	}

	/// @brief 
	/// @param bScaleHeight 
	void AxisLayout::SetConsiderScaleHeight(bool considerScaleHeight)
	{
		if (_considerScaleHeight != considerScaleHeight)
		{
			_considerScaleHeight = considerScaleHeight;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	float AxisLayout::GetSpacing() const
	{
		return _spacing;
	}

	/// @brief 
	/// @param spacing 
	void AxisLayout::SetSpacing(float spacing)
	{
		if (_spacing != spacing)
		{
			_spacing = spacing;
			SetDirty();
		}
	}

	/// @brief 
	/// @return 
	bool AxisLayout::GetReverseArrangement() const
	{
		return _reverseArrangement;
	}

	/// @brief 
	/// @param bScaleHeight 
	void AxisLayout::SetReverseArrangement(bool reverseArrangement)
	{
		if (_reverseArrangement != reverseArrangement)
		{
			_reverseArrangement = reverseArrangement;
			SetDirty();
		}
	}

	/// @brief 
	/// @param currentAxis 
	/// @param mainAxis 
	/// @param totalMin 
	/// @param totalPreferred 
	/// @param totalFlexible 
	void AxisLayout::CalculateSizesForAxis(Axis currentAxis, Axis mainAxis, float& totalMin, float& totalPreferred, float& totalFlexible)
	{
		Node* node = GetOwner()->GetComponent<Node>();
		if (node == nullptr)
		{
			return;
		}

		float combinedPadding;
		bool controlSize;
		bool forceExpand;
		bool useScale;

		if (currentAxis == Axis::Horizontal)
		{
			combinedPadding = GetPadding().GetLeft() + GetPadding().GetRight();
			controlSize = GetControlWidth();
			forceExpand = GetForceExpandWidth();
			useScale = GetConsiderScaleWidth();
		}
		else
		{
			combinedPadding = GetPadding().GetTop() + GetPadding().GetBottom();
			controlSize = GetControlHeight();
			forceExpand = GetForceExpandHeight();
			useScale = GetConsiderScaleHeight();
		}

		totalMin = combinedPadding;
		totalPreferred = combinedPadding;
		totalFlexible = 0.0f;

		float spacing = GetSpacing();

		const Vector<WeakPtr<Node>>& drivenNodes = GetDrivenNodes();
		uint32_t drivenNodeCount = (uint32_t)drivenNodes.size();

		if (node != nullptr)
		{
			for (uint32_t drivenNodeIndex = 0; drivenNodeIndex < drivenNodeCount; ++drivenNodeIndex)
			{
				Node* childNode = drivenNodes[drivenNodeIndex].Get();

				float min = 0.0f;
				float preferred = 0.0f;
				float flexible = 0.0f;

				GetChildSizes(childNode, currentAxis, controlSize, forceExpand, min, preferred, flexible);

				if (useScale == true)
				{
					float scaleFactor = (currentAxis == Axis::Horizontal) ? childNode->GetScale().GetX() : childNode->GetScale().GetY();

					min *= scaleFactor;
					preferred *= scaleFactor;
					flexible *= scaleFactor;
				}

				if (currentAxis == mainAxis)
				{
					totalMin += min + spacing;
					totalPreferred += preferred + spacing;

					totalFlexible += flexible;
				}
				else
				{
					totalMin = std::max(min + combinedPadding, totalMin);
					totalPreferred = std::max(preferred + combinedPadding, totalPreferred);

					totalFlexible = std::max(flexible, totalFlexible);
				}
			}

			if (currentAxis == mainAxis)
			{
				totalMin -= spacing;
				totalPreferred -= spacing;
			}

			totalPreferred = std::max(totalMin, totalPreferred);
		}

		if (currentAxis == Axis::Horizontal)
		{
			SetTotalSizeX(totalMin, totalPreferred, totalFlexible);
		}
		else if (currentAxis == Axis::Vertical)
		{
			SetTotalSizeY(totalMin, totalPreferred, totalFlexible);
		}
	}

	/// @brief 
	/// @param currentAxis 
	/// @param mainAxis 
	/// @param totalMin 
	/// @param totalPreferred 
	/// @param totalFlexible 
	void AxisLayout::PlaceAndResizeChildren(Axis currentAxis, Axis mainAxis, float totalMin, float totalPreferred, float totalFlexible)
	{
		Node* node = GetOwner()->GetComponent<Node>();
		if (node == nullptr)
		{
			return;
		}

		float combinedPadding;
		bool controlSize;
		bool forceExpand;
		bool useScale;

		float pos;

		if (currentAxis == Axis::Horizontal)
		{
			combinedPadding = GetPadding().GetLeft() + GetPadding().GetRight();
			controlSize = GetControlWidth();
			forceExpand = GetForceExpandWidth();
			useScale = GetConsiderScaleWidth();

			pos = GetPadding().GetLeft();
		}
		else
		{
			combinedPadding = GetPadding().GetTop() + GetPadding().GetBottom();
			controlSize = GetControlHeight();
			forceExpand = GetForceExpandHeight();
			useScale = GetConsiderScaleHeight();

			pos = GetPadding().GetTop();
		}

		float spacing = GetSpacing();

		float availableSize = (currentAxis == Axis::Horizontal) ? node->ComputeSize().GetX() : node->ComputeSize().GetY();

		float alignmentMultiplier = GetAlignmentMultiplier(currentAxis);

		if (currentAxis == mainAxis)
		{
			float itemFlexibleMultiplier = 0.0f;
			float surplusSpace = availableSize - totalPreferred;

			if (surplusSpace > 0.0f)
			{
				if (totalFlexible == 0.0f)
				{
					pos = GetStartOffset(currentAxis, totalPreferred - combinedPadding, availableSize);
				}
				else if (totalFlexible > 0.0f)
				{
					itemFlexibleMultiplier = surplusSpace / totalFlexible;
				}
			}
			else
			{
				pos = GetStartOffset(currentAxis, totalPreferred - combinedPadding, availableSize);
			}

			float minMaxLerp = 0.0f;
			if (totalMin != totalPreferred)
			{
				minMaxLerp = std::clamp<float>((availableSize - totalMin) / (totalPreferred - totalMin), 0.0f, 1.0f);
			}

			const Vector<WeakPtr<Node>>& drivenNodes = GetDrivenNodes();
			uint32_t drivenNodeCount = (uint32_t)drivenNodes.size();

			for (uint32_t drivenNodeIndex = 0; drivenNodeIndex < drivenNodeCount; ++drivenNodeIndex)
			{
				uint32_t effectiveNodeIndex = drivenNodeIndex;
				if (_reverseArrangement == true)
				{
					effectiveNodeIndex = drivenNodeCount - 1 - drivenNodeIndex;
				}

				Node* childNode = drivenNodes[effectiveNodeIndex].Get();

				float min = 0.0f;
				float preferred = 0.0f;
				float flexible = 0.0f;

				GetChildSizes(childNode, currentAxis, controlSize, forceExpand, min, preferred, flexible);

				float scaleFactor = 1.0f;

				if (useScale == true)
				{
					scaleFactor = (currentAxis == Axis::Horizontal) ? childNode->GetScale().GetX() : childNode->GetScale().GetY();
				}

				float childSize = std::lerp(min, std::max(min, preferred), minMaxLerp);

				childSize += flexible * itemFlexibleMultiplier;

				if (controlSize == true)
				{
					SetChildAlongAxisWithScale(childNode, currentAxis, pos, childSize, scaleFactor);
				}
				else
				{
					float childDeltaSize = (currentAxis == Axis::Horizontal) ? childNode->GetDeltaSize().GetX() : childNode->GetDeltaSize().GetY();
					float offsetInCell = (childSize - childDeltaSize) * alignmentMultiplier;
					SetChildAlongAxisWithScale(childNode, currentAxis, pos + offsetInCell, scaleFactor);
				}

				pos += childSize * scaleFactor + spacing;
			}
		}
		else
		{
			float innerSize = availableSize - combinedPadding;

			const Vector<WeakPtr<Node>>& drivenNodes = GetDrivenNodes();
			uint32_t drivenNodeCount = (uint32_t)drivenNodes.size();

			for (uint32_t drivenNodeIndex = 0; drivenNodeIndex < drivenNodeCount; ++drivenNodeIndex)
			{
				uint32_t effectiveNodeIndex = drivenNodeIndex;
				if (_reverseArrangement == true)
				{
					effectiveNodeIndex = drivenNodeCount - 1 - drivenNodeIndex;
				}

				Node* childNode = drivenNodes[effectiveNodeIndex].Get();

				float min = 0.0f;
				float preferred = 0.0f;
				float flexible = 0.0f;

				GetChildSizes(childNode, currentAxis, controlSize, forceExpand, min, preferred, flexible);

				float scaleFactor = 1.0f;

				if (useScale == true)
				{
					scaleFactor = (currentAxis == Axis::Horizontal) ? childNode->GetScale().GetX() : childNode->GetScale().GetY();
				}

				float requiredSpace;

				if (flexible > 0.0f)
				{
					requiredSpace = std::clamp<float>(innerSize, min, availableSize);
				}
				else
				{
					requiredSpace = std::clamp<float>(innerSize, min, preferred);
				}

				float startOffset = GetStartOffset(currentAxis, requiredSpace * scaleFactor, availableSize);

				if (controlSize == true)
				{
					SetChildAlongAxisWithScale(childNode, currentAxis, startOffset, requiredSpace, scaleFactor);
				}
				else
				{
					float childDeltaSize = (currentAxis == Axis::Horizontal) ? childNode->GetDeltaSize().GetX() : childNode->GetDeltaSize().GetY();
					float offsetInCell = (requiredSpace - childDeltaSize) * alignmentMultiplier;
					SetChildAlongAxisWithScale(childNode, currentAxis, startOffset + offsetInCell, scaleFactor);
				}
			}
		}
	}

	/// @brief 
	/// @param childNode 
	/// @param axis 
	/// @param controlSize 
	/// @param forceExpand 
	/// @param min 
	/// @param preferred 
	/// @param flexible 
	void AxisLayout::GetChildSizes(Node* childNode, Axis axis, bool controlSize, bool forceExpand, float& min, float& preferred, float& flexible)
	{
		if (controlSize == false)
		{
			min = (axis == Axis::Horizontal) ? childNode->GetDeltaSize().GetX() : childNode->GetDeltaSize().GetY();
			preferred = min;
			flexible = 0;
		}
		else
		{
			LayoutElement* layoutElement = GetOwner()->GetComponent<LayoutElement>();
			if (layoutElement != nullptr)
			{
				min = (axis == Axis::Horizontal) ? layoutElement->GetMinSize().GetX() : layoutElement->GetMinSize().GetY();
				preferred = (axis == Axis::Horizontal) ? layoutElement->GetPreferredSize().GetX() : layoutElement->GetPreferredSize().GetY();
				flexible = (axis == Axis::Horizontal) ? layoutElement->GetFlexibleSize().GetX() : layoutElement->GetFlexibleSize().GetY();
			}
		}

		if (forceExpand == true)
		{
			flexible = std::max(flexible, 1.0f);
		}
	}
}

#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Layout/Layout.hpp"

#include "HodEngine/UI/Canvas.hpp"

namespace hod::ui
{
	DESCRIBE_REFLECTED_ENUM(Layout::Axis, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Layout::Axis::Horizontal, "Horizontal");
		reflectionDescriptor.AddEnumValue(Layout::Axis::Vertical, "Vertical");
	}

	DESCRIBE_REFLECTED_ENUM(Layout::Alignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Layout::Alignment::TopLeft, "TopLeft");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::TopCenter, "TopCenter");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::TopRight, "TopRight");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::MiddleLeft, "MiddleLeft");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::MiddleCenter, "MiddleCenter");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::MiddleRight, "MiddleRight");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::BottomLeft, "BottomLeft");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::BottomCenter, "BottomCenter");
		reflectionDescriptor.AddEnumValue(Layout::Alignment::BottomRight, "BottomRight");
	}

	DESCRIBE_REFLECTED_CLASS(Layout, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Layout::_padding, "_padding", &Layout::SetPadding);
		AddPropertyT(reflectionDescriptor, &Layout::_alignment, "_alignment", &Layout::SetAlignment);
	}

	Layout::Layout()
	: Rebuildable()
	, _onDrivenNodeChangedSlot(std::bind(&Layout::OnDrivenNodeChanged, this))
	, _onDrivenNodeLayoutElementChangedSlot(std::bind(&Layout::OnDrivenNodeLayoutElementChanged, this, std::placeholders::_1))
	, _onChildrenChangedSlot(std::bind(&Layout::OnChildrenChanged, this))
	{
	}

	void Layout::OnEnable()
	{
		GetOwner()->GetChildrenChangedEvent() += _onChildrenChangedSlot;

		Node* node = GetOwner()->GetComponent<Node>();
		if (node != nullptr)
		{
			node->GetPropertyChangedEvent() += _onDrivenNodeChangedSlot;
		}

		SetDirty();
	}

	/// @brief
	void Layout::OnDisable()
	{
		//_onChildrenChangedSlot.DisconnectAll();
		//_onDrivenNodeChangedSlot.DisconnectAll(); // TODO Event::Slot DisconnectAll
	}

	/// @brief
	bool Layout::Rebuild()
	{
		if (_drivenNodesDirty == true)
		{
			_drivenNodesDirty = false;
			_drivenNodes.Clear();
			//_onDrivenNodeChangedSlot.DisconnectAll(); // TODO Event::Slot DisconnectAll
			//_onDrivenNodeLayoutElementChangedSlot.DisconnectAll(); // TODO Event::Slot DisconnectAll

			Node* node = GetOwner()->GetComponent<Node>();
			if (node != nullptr)
			{
				node->GetPropertyChangedEvent() += _onDrivenNodeChangedSlot;
			}

			for (const game::WeakEntity& weakChild : GetOwner()->GetChildren())
			{
				game::Entity* child = weakChild.Lock();
				Node*         childNode = child->GetComponent<Node>();
				if (childNode == nullptr)
				{
					OUTPUT_ERROR("UI::Layout: missing UI::Node on child {}", weakChild.Lock()->GetName());
					continue;
				}

				if (childNode->IsEnabledInHierarchy() == false)
				{
					continue;
				}

				LayoutElement* layoutElement = child->GetComponent<LayoutElement>();
				if (layoutElement != nullptr)
				{
					layoutElement->GetPropertyChangedEvent() += _onDrivenNodeLayoutElementChangedSlot;
					if (layoutElement->GetIgnoreLayout() == true)
					{
						continue;
					}
				}

				childNode->GetPropertyChangedEvent() += _onDrivenNodeChangedSlot;
				_drivenNodes.push_back(childNode);
			}
		}

		ComputeChildrenPositionAndSize();

		return true;
	}

	/// @brief
	/// @return
	const Padding& Layout::GetPadding() const
	{
		return _padding;
	}

	/// @brief
	/// @param padding
	void Layout::SetPadding(const Padding& padding)
	{
		if (_padding._vector4 != padding._vector4)
		{
			_padding = padding;
			SetDirty();
		}
	}

	/// @brief
	/// @return
	Layout::Alignment Layout::GetAlignment() const
	{
		return _alignment;
	}

	/// @brief
	/// @param alignment
	void Layout::SetAlignment(Alignment alignment)
	{
		if (_alignment != alignment)
		{
			_alignment = alignment;
			SetDirty();
		}
	}

	/// @brief
	/// @param axis
	/// @param requiredSpaceWithoutPadding
	/// @param availableSpace
	/// @return
	float Layout::GetStartOffset(Axis axis, float requiredSpaceWithoutPadding, float availableSpace)
	{
		float combinedPadding;

		if (axis == Axis::Horizontal)
		{
			combinedPadding = _padding.GetLeft() + _padding.GetRight();
		}
		else
		{
			combinedPadding = _padding.GetTop() + _padding.GetBottom();
		}

		float requiredSpace = requiredSpaceWithoutPadding + combinedPadding;
		float surplusSpace = availableSpace - requiredSpace;
		float alignmentMultiplier = GetAlignmentMultiplier(axis);

		float startOffset;

		if (axis == Axis::Horizontal)
		{
			startOffset = _padding.GetLeft();
		}
		else
		{
			startOffset = _padding.GetTop();
		}

		return startOffset + surplusSpace * alignmentMultiplier;
	}

	/// @brief
	/// @param axis
	/// @return
	float Layout::GetAlignmentMultiplier(Axis axis)
	{
		if (axis == Axis::Horizontal)
		{
			return ((int32_t)GetAlignment() % 3) * 0.5f;
		}
		else
		{
			return ((int32_t)GetAlignment() / 3) * 0.5f;
		}
	}

	/// @brief
	/// @param node
	/// @param axis
	/// @param position
	void Layout::SetChildAlongAxis(Node* node, Axis axis, float position)
	{
		node->SetAnchorMin(Vector2(0.0f, 1.0f));
		node->SetAnchorMax(Vector2(0.0f, 1.0f));

		Vector2 anchoredPos = node->GetPosition();

		if (axis == Axis::Horizontal)
		{
			anchoredPos.SetX(position + node->GetDeltaSize().GetX() * node->GetPivot().GetX());
		}
		else
		{
			anchoredPos.SetY(-position - node->GetDeltaSize().GetY() * (1.0f - node->GetPivot().GetY()));
		}

		node->SetPosition(anchoredPos);
	}

	/// @brief
	/// @param node
	/// @param axis
	/// @param position
	/// @param scaleFactor
	void Layout::SetChildAlongAxisWithScale(Node* node, Axis axis, float position, float scaleFactor)
	{
		node->SetAnchorMin(Vector2(0.0f, 1.0f));
		node->SetAnchorMax(Vector2(0.0f, 1.0f));

		Vector2 anchoredPos = node->GetPosition();
		if (axis == Axis::Horizontal)
		{
			anchoredPos.SetX(position + node->GetDeltaSize().GetX() * node->GetPivot().GetX() * scaleFactor);
		}
		else
		{
			anchoredPos.SetY(-position - node->GetDeltaSize().GetY() * (1.0f - node->GetPivot().GetY()) * scaleFactor);
		}

		node->SetPosition(anchoredPos);
	}

	/// @brief
	/// @param node
	/// @param axis
	/// @param position
	/// @param Size
	/// @param scaleFactor
	void Layout::SetChildAlongAxisWithScale(Node* node, Axis axis, float position, float Size, float scaleFactor)
	{
		node->SetAnchorMin(Vector2(0.0f, 1.0f));
		node->SetAnchorMax(Vector2(0.0f, 1.0f));

		Vector2 deltaSize = node->GetDeltaSize();
		Vector2 anchoredPos = node->GetPosition();

		if (axis == Axis::Horizontal)
		{
			deltaSize.SetX(Size);
			anchoredPos.SetX(position + Size * node->GetPivot().GetX() * scaleFactor);
		}
		else
		{
			deltaSize.SetY(Size);
			anchoredPos.SetY(-position - Size * (1.0f - node->GetPivot().GetY()) * scaleFactor);
		}

		node->SetDeltaSize(deltaSize);
		node->SetPosition(anchoredPos);
	}

	/// @brief
	/// @param minSize
	/// @param preferredSize
	/// @param flexibleSize
	void Layout::SetTotalSizeX(float minSize, float preferredSize, float flexibleSize)
	{
		minSize = std::max(0.0f, minSize);
		preferredSize = std::max(0.0f, preferredSize);
		flexibleSize = std::max(0.0f, flexibleSize);

		if (_totalMinSize.GetX() != minSize || _totalPreferredSize.GetX() != preferredSize || _totalFlexibleSize.GetX() != flexibleSize)
		{
			_totalMinSize.SetX(minSize);
			_totalPreferredSize.SetX(preferredSize);
			_totalFlexibleSize.SetX(flexibleSize);

			_sizeChangedEvent.Emit(ChangedSizeProperty::TotalSizeX);
		}
	}

	/// @brief
	/// @param minSize
	/// @param preferredSize
	/// @param flexibleSize
	void Layout::SetTotalSizeY(float minSize, float preferredSize, float flexibleSize)
	{
		minSize = std::max(0.0f, minSize);
		preferredSize = std::max(0.0f, preferredSize);
		flexibleSize = std::max(0.0f, flexibleSize);

		if (_totalMinSize.GetY() != minSize || _totalPreferredSize.GetY() != preferredSize || _totalFlexibleSize.GetY() != flexibleSize)
		{
			_totalMinSize.SetY(minSize);
			_totalPreferredSize.SetY(preferredSize);
			_totalFlexibleSize.SetY(flexibleSize);

			_sizeChangedEvent.Emit(ChangedSizeProperty::TotalSizeY);
		}
	}

	/// @brief
	/// @return
	const Vector2& Layout::GetTotalMinSize() const
	{
		return _totalMinSize;
	}

	/// @brief
	/// @return
	const Vector2& Layout::GetTotalPreferredSize() const
	{
		return _totalPreferredSize;
	}

	/// @brief
	/// @return
	const Vector2& Layout::GetTotalFlexibleSize() const
	{
		return _totalFlexibleSize;
	}

	/// @brief
	void Layout::OnDrivenNodeChanged()
	{
		SetDirty();
	}

	/// @brief
	void Layout::OnDrivenNodeLayoutElementChanged(LayoutElement::ChangedProperty changedProperty)
	{
		SetDirty();

		if (changedProperty == LayoutElement::ChangedProperty::IgnoreLayout)
		{
			SetDirty();
			_drivenNodesDirty = true;
		}
	}

	/// @brief
	void Layout::OnChildrenChanged()
	{
		SetDirty();
		_drivenNodesDirty = true;
	}

	/// @brief
	/// @return
	const Vector<WeakPtr<Node>>& Layout::GetDrivenNodes() const
	{
		return _drivenNodes;
	}

	/// @brief
	/// @return
	Layout::SizeChangedEvent& Layout::GetSizeChangedEvent()
	{
		return _sizeChangedEvent;
	}
}

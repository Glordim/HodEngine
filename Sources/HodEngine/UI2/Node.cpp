#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Assert.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"
#include "HodEngine/UI2/LayoutParamsFactory.hpp"
#include "HodEngine/UI2/Layout.hpp"
#include "HodEngine/UI2/LayoutFactory.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

#include <algorithm>

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(Node, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Node::_rotation, "Rotation", &Node::SetRotation);
		AddPropertyT(reflectionDescriptor, &Node::_scale, "Scale", &Node::SetScale);
		AddPropertyT(reflectionDescriptor, &Node::_origin, "Origin", &Node::SetOrigin);

		AddPropertyT(reflectionDescriptor, &Node::_desiredSize, "DesiredSize", &Node::SetDesiredSize);
	}

	Node::~Node()
	{
		DefaultAllocator::GetInstance().Delete(_layout);

		for (Node* child : _children)
		{
			DefaultAllocator::GetInstance().Delete(child->_layoutParams);
			DefaultAllocator::GetInstance().Delete(child);
		}
	}

	void Node::AddChild(Node* child)
	{
		AddChild(child, CreateDefaultLayoutParams());
	}

	void Node::AddChild(Node* child, LayoutParams* layoutParams)
	{
		Assert(child != nullptr);
		Assert(layoutParams != nullptr);

		child->_parent = this;
		child->_layoutParams = layoutParams;
		layoutParams->_node = child;

		_children.PushBack(child);

		// child->MarkMeasureAsDirty() alone isn't enough: a freshly constructed Node already starts
		// with its Measure flag set (see _dirtyFlags' default), so that call's own dirty check finds
		// it already dirty and never walks up to notify `this` — which just gained a new child and
		// needs to know regardless of child's prior state (also covers re-parenting an already-clean
		// node, which still needs `this` in particular, not just its old parent, marked dirty).
		child->MarkMeasureAsDirty();
		MarkMeasureAsDirty();
	}

	void Node::RemoveChild(Node* child)
	{
		auto it = std::find(_children.Begin(), _children.End(), child);
		if (it != _children.End())
		{
			DefaultAllocator::GetInstance().Delete(child->_layoutParams);
			child->_layoutParams = nullptr;
			child->_parent = nullptr;

			_children.Erase(it);

			MarkMeasureAsDirty();
		}
	}

	/// @brief
	/// @return
	float Node::GetRotation() const
	{
		return _rotation;
	}

	/// @brief
	/// @param rotation
	void Node::SetRotation(float rotation)
	{
		if (_rotation != rotation)
		{
			_rotation = rotation;
			MarkArrangeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& Node::GetScale() const
	{
		return _scale;
	}

	/// @brief
	/// @param scale
	void Node::SetScale(const Vector2& scale)
	{
		if (_scale != scale)
		{
			_scale = scale;
			MarkArrangeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& Node::GetOrigin() const
	{
		return _origin;
	}

	/// @brief
	/// @param origin
	void Node::SetOrigin(const Vector2& origin)
	{
		if (_origin != origin)
		{
			_origin = origin;
			MarkArrangeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const Vector2& Node::GetDesiredSize() const
	{
		return _desiredSize;
	}

	/// @brief
	/// @param desiredSize
	void Node::SetDesiredSize(const Vector2& desiredSize)
	{
		if (_desiredSize != desiredSize)
		{
			_desiredSize = desiredSize;
			MarkMeasureAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	void Node::MarkMeasureAsDirty()
	{
		if ((_dirtyFlags & (uint8_t)DirtyFlag::Measure) == 0)
		{
			_dirtyFlags |= (uint8_t)DirtyFlag::Measure;

			if (_parent != nullptr)
			{
				_parent->MarkMeasureAsDirty();
			}
		}
	}

	/// @brief
	void Node::MarkArrangeAsDirty()
	{
		if ((_dirtyFlags & (uint8_t)DirtyFlag::Arrange) == 0)
		{
			_dirtyFlags |= (uint8_t)DirtyFlag::Arrange;

			if (_parent != nullptr)
			{
				_parent->MarkArrangeAsDirty();
			}
		}
	}

	/// @brief
	void Node::MarkCanvasMatrixAsDirty()
	{
		if ((_dirtyFlags & (uint8_t)DirtyFlag::CanvasMatrix) == 0)
		{
			_dirtyFlags |= (uint8_t)DirtyFlag::CanvasMatrix;
			for (uint32_t childIndex = 0; childIndex < _children.Size(); ++childIndex)
			{
				Node* childNode = _children[childIndex];
				if (childNode != nullptr)
				{
					childNode->MarkCanvasMatrixAsDirty();
				}
			}
		}
	}

	/// @brief
	/// @return
	bool Node::NeedsMeasure() const
	{
		return (_dirtyFlags & (uint8_t)DirtyFlag::Measure) != 0 || (_layoutParams != nullptr && _layoutParams->DependsOnParentSize());
	}

	/// @brief
	/// @return
	bool Node::NeedsArrange() const
	{
		return (_dirtyFlags & (uint8_t)DirtyFlag::Arrange) != 0 || (_layoutParams != nullptr && _layoutParams->DependsOnParentSize());
	}

	/// @brief
	/// @param measuredSize
	void Node::FinishMeasure(const Vector2& measuredSize)
	{
		Vector2 previousMeasuredSize = _measuredSize;
		_measuredSize = measuredSize;

		if (_measuredSize != previousMeasuredSize)
		{
			MarkArrangeAsDirty();
		}

		_dirtyFlags &= ~((uint8_t)DirtyFlag::Measure);
	}

	/// @brief
	/// @param finalRect
	/// @return
	Rect Node::BeginArrange(const Rect& finalRect)
	{
		_size = finalRect._size;
		_position = finalRect._position + finalRect._size * 0.5f;

		ComputeLocalMatrix(_position, _size, _localMatrix);
		MarkCanvasMatrixAsDirty();

		_dirtyFlags &= ~((uint8_t)DirtyFlag::Arrange);

		Rect contentRect;
		contentRect._position = _size * -0.5f;
		contentRect._size = _size;
		return contentRect;
	}

	/// @brief
	/// @param availableSize
	/// @return
	Vector2 Node::Measure(const Vector2& availableSize)
	{
		if (NeedsMeasure() == false)
		{
			return _measuredSize;
		}

		Vector2 contentDerivedSize;
		if (_layout != nullptr)
		{
			contentDerivedSize = _layout->Measure(*this, availableSize);
		}
		else if (_children.Empty() == false)
		{
			Vector2 maxChildSize = Vector2::Zero;
			for (Node* child : _children)
			{
				if (child == nullptr)
				{
					continue;
				}

				Margin  childMargin = child->_layoutParams != nullptr ? child->_layoutParams->GetMargin() : Margin();
				Vector2 marginSize(childMargin._left + childMargin._right, childMargin._top + childMargin._bottom);

				Vector2 childAvailable(std::max(availableSize.GetX() - marginSize.GetX(), 0.0f), std::max(availableSize.GetY() - marginSize.GetY(), 0.0f));

				Vector2 childSize = child->Measure(childAvailable) + marginSize;

				maxChildSize = Vector2(std::max(maxChildSize.GetX(), childSize.GetX()), std::max(maxChildSize.GetY(), childSize.GetY()));
			}

			contentDerivedSize = maxChildSize;
		}
		else
		{
			contentDerivedSize = MeasureContent(availableSize);
		}

		FinishMeasure(ResolveMeasuredSize(availableSize, contentDerivedSize));
		return _measuredSize;
	}

	/// @brief
	/// @param availableSize
	/// @param contentDerivedSize
	/// @return
	Vector2 Node::ResolveMeasuredSize(const Vector2& availableSize, const Vector2& contentDerivedSize) const
	{
		Vector2 measuredSize;
		if (_layoutParams != nullptr && _layoutParams->TryComputeSize(availableSize, _desiredSize, measuredSize))
		{
			return measuredSize;
		}

		measuredSize = contentDerivedSize;
		if (_layoutParams != nullptr)
		{
			const Vector2& minSize = _layoutParams->GetMinSize();
			const Vector2& maxSize = _layoutParams->GetMaxSize();
			measuredSize = Vector2(std::clamp(measuredSize.GetX(), minSize.GetX(), maxSize.GetX()), std::clamp(measuredSize.GetY(), minSize.GetY(), maxSize.GetY()));
		}
		return measuredSize;
	}

	/// @brief
	/// @param availableSize
	/// @return
	Vector2 Node::MeasureContent(const Vector2& availableSize) const
	{
		(void)availableSize;
		return _desiredSize;
	}

	/// @brief
	/// @param finalRect
	void Node::Arrange(const Rect& finalRect)
	{
		if (NeedsArrange() == false)
		{
			return;
		}

		Rect contentRect = BeginArrange(finalRect);

		if (_layout != nullptr)
		{
			_layout->Arrange(*this, contentRect);
		}
		else
		{
			for (Node* child : _children)
			{
				if (child != nullptr)
				{
					ArrangeChildInRect(child, contentRect);
				}
			}
		}
	}

	/// @brief
	/// @param child
	/// @param parentContentRect
	void Node::ArrangeChildInRect(Node* child, const Rect& parentContentRect)
	{
		LayoutParams* childLayoutParams = child->_layoutParams;

		Vector2 overriddenPosition;
		if (childLayoutParams != nullptr && childLayoutParams->TryComputePosition(parentContentRect, child->_measuredSize, overriddenPosition))
		{
			Rect childFinalRect;
			childFinalRect._size = child->_measuredSize;
			childFinalRect._position = overriddenPosition - child->_measuredSize * 0.5f;
			child->Arrange(childFinalRect);
			return;
		}

		Margin  margin = childLayoutParams != nullptr ? childLayoutParams->GetMargin() : Margin();
		Vector2 minSize = childLayoutParams != nullptr ? childLayoutParams->GetMinSize() : Vector2::Zero;
		Vector2 maxSize = childLayoutParams != nullptr ? childLayoutParams->GetMaxSize() : Vector2::Infinity;
		uint8_t hAlign = (uint8_t)(childLayoutParams != nullptr ? childLayoutParams->GetHAlign() : LayoutParams::HAlign::Stretch);
		uint8_t vAlign = (uint8_t)(childLayoutParams != nullptr ? childLayoutParams->GetVAlign() : LayoutParams::VAlign::Stretch);

		Rect insetRect = parentContentRect.Inset(margin);

		float x, width, y, height;
		ComputeAlignedExtent(insetRect._position.GetX(), insetRect._size.GetX(), child->_measuredSize.GetX(), minSize.GetX(), maxSize.GetX(), hAlign, true, x, width);
		ComputeAlignedExtent(insetRect._position.GetY(), insetRect._size.GetY(), child->_measuredSize.GetY(), minSize.GetY(), maxSize.GetY(), vAlign, false, y, height);

		Rect childFinalRect;
		childFinalRect._position = Vector2(x, y);
		childFinalRect._size = Vector2(width, height);

		child->Arrange(childFinalRect);
	}

	/// @brief
	/// @param availableStart
	/// @param availableExtent
	/// @param desiredExtent
	/// @param minExtent
	/// @param maxExtent
	/// @param alignIndex
	/// @param startIsIndex0
	/// @param outOffset
	/// @param outExtent
	void Node::ComputeAlignedExtent(
		float availableStart, float availableExtent, float desiredExtent, float minExtent, float maxExtent, uint8_t alignIndex, bool startIsIndex0, float& outOffset, float& outExtent)
	{
		constexpr uint8_t centerIndex = 1;
		constexpr uint8_t stretchIndex = 3;

		if (alignIndex == stretchIndex)
		{
			outExtent = std::clamp(availableExtent, minExtent, maxExtent);
			outOffset = availableStart;
			return;
		}

		outExtent = std::clamp(desiredExtent, minExtent, maxExtent);

		if (alignIndex == centerIndex)
		{
			outOffset = availableStart + (availableExtent - outExtent) * 0.5f;
			return;
		}

		bool isStartSide = (alignIndex == 0) ? startIsIndex0 : (startIsIndex0 == false);
		outOffset = isStartSide ? availableStart : (availableStart + availableExtent - outExtent);
	}

	/// @brief
	/// @return
	const Vector2& Node::GetMeasuredSize() const
	{
		return _measuredSize;
	}

	/// @brief
	/// @return
	const Vector2& Node::GetSize() const
	{
		return _size;
	}

	/// @brief
	/// @return
	const Vector2& Node::GetPosition() const
	{
		return _position;
	}

	/// @brief
	/// @return
	const Matrix4& Node::GetLocalMatrix() const
	{
		return _localMatrix;
	}

	/// @brief
	/// @param position
	/// @param size
	/// @param localMatrix
	void Node::ComputeLocalMatrix(const Vector2& position, const Vector2& size, Matrix4& localMatrix) const
	{
		const Vector2& pixelPivot = (_origin - Vector2(0.5f, 0.5f)) * size;

		Matrix4 anchor = Matrix4::Translation(pixelPivot);
		Matrix4 inverseAnchor = Matrix4::Translation(-pixelPivot);

		Matrix4 translation = Matrix4::Translation(position);
		Matrix4 rotation = Matrix4::Rotation(_rotation);
		Matrix4 scaling = Matrix4::Scale(_scale);

		localMatrix = inverseAnchor * scaling * rotation * anchor * translation * inverseAnchor;
	}

	/// @brief
	/// @return
	const Matrix4& Node::ComputeCanvasMatrix()
	{
		if (_dirtyFlags & (uint8_t)DirtyFlag::CanvasMatrix)
		{
			if (_parent != nullptr)
			{
				_canvasMatrix = _localMatrix * _parent->ComputeCanvasMatrix();
			}
			else
			{
				_canvasMatrix = _localMatrix;
			}

			_dirtyFlags &= ~((uint8_t)DirtyFlag::CanvasMatrix);
		}

		return _canvasMatrix;
	}

	Matrix4 Node::ComputeWorldMatrix()
	{
		// Scene has no render-mode/scale-mode concept yet (unlike the old UI::Canvas), so canvas
		// space is world space for now; revisit once that concept is designed.
		return ComputeCanvasMatrix();
	}

	/// @brief
	/// @return
	Node* Node::GetParent() const
	{
		return _parent;
	}

	const Vector<Node*>& Node::GetChildren() const
	{
		return _children;
	}

	LayoutParams* Node::GetLayoutParams() const
	{
		return _layoutParams;
	}

	LayoutParams* Node::CreateDefaultLayoutParams() const
	{
		if (_layout != nullptr)
		{
			return _layout->CreateDefaultLayoutParams();
		}
		return DefaultAllocator::GetInstance().New<AnchoredLayoutParams>();
	}

	void Node::SetLayout(Layout* layout)
	{
		if (_layout != nullptr)
		{
			DefaultAllocator::GetInstance().Delete(_layout);
		}

		_layout = layout;
		if (_layout != nullptr)
		{
			_layout->_node = this;
		}

		// Swapping the arrangement algorithm can change both this node's own content-derived size
		// and how its children end up placed, even when that size doesn't change.
		MarkMeasureAsDirty();
		MarkArrangeAsDirty();
	}

	Layout* Node::GetLayout() const
	{
		return _layout;
	}

	void Node::SetZOrder(int32_t zOrder)
	{
		_zOrder = zOrder;
	}

	int32_t Node::GetZOrder() const
	{
		return _zOrder;
	}

	Event<>& Node::GetPropertyChangedEvent()
	{
		return _propertyChangedEvent;
	}

	bool Node::SerializeInDocument(DocumentNode& documentNode)
	{
		if (Serializer::Serialize(*this, documentNode) == false)
		{
			return false;
		}

		if (_layout != nullptr)
		{
			DocumentNode& layoutNode = documentNode.AddChild("Layout");
			layoutNode.AddChild("Type").SetUInt64(_layout->GetReflectionDescriptorV().GetType());
			if (Serializer::Serialize(_layout, layoutNode) == false)
			{
				return false;
			}
		}

		DocumentNode& childrenNode = documentNode.AddChild("Children");
		for (Node* child : _children)
		{
			DocumentNode& childNode = childrenNode.AddChild("");

			if (child->_layoutParams != nullptr)
			{
				DocumentNode& layoutParamsNode = childNode.AddChild("LayoutParams");
				layoutParamsNode.AddChild("Type").SetUInt64(child->_layoutParams->GetReflectionDescriptorV().GetType());
				if (Serializer::Serialize(child->_layoutParams, layoutParamsNode) == false)
				{
					return false;
				}
			}

			if (child->SerializeInDocument(childNode) == false)
			{
				return false;
			}
		}

		return true;
	}

	bool Node::DeserializeFromDocument(const DocumentNode& documentNode)
	{
		if (Serializer::Deserialize(*this, documentNode) == false)
		{
			return false;
		}

		const DocumentNode* layoutNode = documentNode.GetChild("Layout");
		if (layoutNode != nullptr)
		{
			const DocumentNode* typeNode = layoutNode->GetChild("Type");
			if (typeNode != nullptr)
			{
				const std::map<uint64_t, ReflectionDescriptor*>& descriptors = LayoutFactory::GetInstance()->GetAllDescriptors();
				auto                                              it = descriptors.find(typeNode->GetUInt64());
				if (it != descriptors.end())
				{
					Layout* layout = it->second->CreateInstance<Layout>();
					Serializer::Deserialize(*it->second, layout, *layoutNode);
					SetLayout(layout);
				}
			}
		}

		const DocumentNode* childrenNode = documentNode.GetChild("Children");
		if (childrenNode == nullptr)
		{
			return true;
		}

		const DocumentNode* childNode = childrenNode->GetFirstChild();
		while (childNode != nullptr)
		{
			Node* child = DefaultAllocator::GetInstance().New<Node>();

			LayoutParams*       layoutParams = nullptr;
			const DocumentNode* layoutParamsNode = childNode->GetChild("LayoutParams");
			if (layoutParamsNode != nullptr)
			{
				const DocumentNode* typeNode = layoutParamsNode->GetChild("Type");
				if (typeNode != nullptr)
				{
					const std::map<uint64_t, ReflectionDescriptor*>& descriptors = LayoutParamsFactory::GetInstance()->GetAllDescriptors();
					auto                                              it = descriptors.find(typeNode->GetUInt64());
					if (it != descriptors.end())
					{
						layoutParams = it->second->CreateInstance<LayoutParams>();
						Serializer::Deserialize(*it->second, layoutParams, *layoutParamsNode);
					}
				}
			}

			if (layoutParams == nullptr)
			{
				layoutParams = CreateDefaultLayoutParams();
			}

			AddChild(child, layoutParams);

			if (child->DeserializeFromDocument(*childNode) == false)
			{
				return false;
			}

			childNode = childNode->GetNextSibling();
		}

		return true;
	}
}

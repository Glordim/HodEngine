#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Assert.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Node.hpp"
#include "HodEngine/UI2/LayoutParams.hpp"
#include "HodEngine/UI2/AnchoredLayoutParams.hpp"
#include "HodEngine/UI2/LayoutParamsFactory.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Serialization/Serializer.hpp>

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

		child->MarkSizeAsDirty();
		child->MarkLocalMatrixAsDirty();
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
			MarkLocalMatrixAsDirty();

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
			MarkLocalMatrixAsDirty();

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
			MarkLocalMatrixAsDirty();

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
			MarkSizeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	void Node::MarkSizeAsDirty()
	{
		_dirtyFlags |= (uint8_t)DirtyFlag::Size;

		for (uint32_t childIndex = 0; childIndex < _children.Size(); ++childIndex)
		{
			Node* childNode = _children[childIndex];
			if (childNode != nullptr && childNode->_layoutParams != nullptr && childNode->_layoutParams->DependsOnParentSize())
			{
				childNode->MarkSizeAsDirty();
				childNode->MarkLocalMatrixAsDirty();
			}
		}
	}

	/// @brief
	void Node::MarkLocalMatrixAsDirty()
	{
		_dirtyFlags |= (uint8_t)DirtyFlag::LocalMatrix;
		MarkCanvasMatrixAsDirty();
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
	const Vector2& Node::ComputeSize()
	{
		if (_dirtyFlags & (uint8_t)DirtyFlag::Size)
		{
			Vector2 previousSize = _size;
			ComputeSize(_parent, _size);

			if (_size != previousSize)
			{
				MarkLocalMatrixAsDirty();
			}

			_dirtyFlags &= ~((uint8_t)DirtyFlag::Size);
		}

		return _size;
	}

	/// @brief
	/// @return
	void Node::ComputeSize(Node* parent, Vector2& size) const
	{
		if (_layoutParams != nullptr)
		{
			Vector2 parentSize = Vector2::Zero;
			if (parent != nullptr)
			{
				parentSize = parent->ComputeSize();
			}

			size = _layoutParams->ComputeSize(parentSize, _desiredSize);
		}
		else
		{
			size = _desiredSize;
		}
	}

	/// @brief
	/// @return
	const Matrix4& Node::ComputeLocalMatrix()
	{
		ComputeSize();
		if (_dirtyFlags & (uint8_t)DirtyFlag::LocalMatrix)
		{
			ComputeLocalMatrix(_parent, _size, _localMatrix);

			_dirtyFlags &= ~((uint8_t)DirtyFlag::LocalMatrix);
		}

		return _localMatrix;
	}

	/// @brief
	/// @return
	void Node::ComputeLocalMatrix(Node* parent, const Vector2& size, Matrix4& localMatrix) const
	{
		Vector2 position = Vector2::Zero;
		if (_layoutParams != nullptr)
		{
			Vector2 parentSize = Vector2::Zero;
			if (parent != nullptr)
			{
				parentSize = parent->ComputeSize();
			}

			position = _layoutParams->ComputePosition(parentSize, size);
		}

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
		ComputeLocalMatrix();
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
		return DefaultAllocator::GetInstance().New<AnchoredLayoutParams>();
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

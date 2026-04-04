#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Canvas.hpp"
#include "HodEngine/UI/Node.hpp"

#include <HodEngine/Game/Entity.hpp>

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Node, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Node::_position, "Position", &Node::SetPosition);
		AddPropertyT(reflectionDescriptor, &Node::_rotation, "Rotation", &Node::SetRotation);
		AddPropertyT(reflectionDescriptor, &Node::_scale, "Scale", &Node::SetScale);

		AddPropertyT(reflectionDescriptor, &Node::_anchorMin, "AnchorMin", &Node::SetAnchorMin);
		AddPropertyT(reflectionDescriptor, &Node::_anchorMax, "AnchorMax", &Node::SetAnchorMax);
		AddPropertyT(reflectionDescriptor, &Node::_pivot, "Pivot", &Node::SetPivot);

		AddPropertyT(reflectionDescriptor, &Node::_deltaSize, "DeltaSize", &Node::SetDeltaSize);
	}

	/// @brief
	void Node::OnEnable()
	{
		game::Entity* owner = GetOwner();
		if (owner != nullptr)
		{
			game::Entity* parentEntity = owner->GetParent().Lock();
			if (parentEntity != nullptr)
			{
				_parent = parentEntity->GetComponent<Node>();
			}
		}
	}

	/// @brief
	/// @return
	const math::Vector2& Node::GetPosition() const
	{
		return _position;
	}

	/// @brief
	/// @param position
	void Node::SetPosition(const math::Vector2& position)
	{
		if (_position != position)
		{
			_position = position;
			MarkLocalMatrixAsDirty();

			_propertyChangedEvent.Emit();
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
	const math::Vector2& Node::GetScale() const
	{
		return _scale;
	}

	/// @brief
	/// @param scale
	void Node::SetScale(const math::Vector2& scale)
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
	const math::Vector2& Node::GetAnchorMin() const
	{
		return _anchorMin;
	}

	/// @brief
	/// @param anchorMin
	void Node::SetAnchorMin(const math::Vector2& anchorMin)
	{
		if (_anchorMin != anchorMin)
		{
			_anchorMin = anchorMin;
			MarkSizeAsDirty();
			MarkLocalMatrixAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const math::Vector2& Node::GetAnchorMax() const
	{
		return _anchorMax;
	}

	/// @brief
	/// @param anchorMax
	void Node::SetAnchorMax(const math::Vector2& anchorMax)
	{
		if (_anchorMax != anchorMax)
		{
			_anchorMax = anchorMax;
			MarkSizeAsDirty();
			MarkLocalMatrixAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const math::Vector2& Node::GetPivot() const
	{
		return _pivot;
	}

	/// @brief
	/// @param pivot
	void Node::SetPivot(const math::Vector2& pivot)
	{
		if (_pivot != pivot)
		{
			_pivot = pivot;
			MarkLocalMatrixAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @return
	const math::Vector2& Node::GetDeltaSize() const
	{
		return _deltaSize;
	}

	/// @brief
	/// @param deltaSize
	void Node::SetDeltaSize(const math::Vector2& deltaSize)
	{
		if (_deltaSize != deltaSize)
		{
			_deltaSize = deltaSize;
			MarkSizeAsDirty();

			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	void Node::MarkSizeAsDirty()
	{
		_dirtyFlags |= (uint8_t)DirtyFlag::Size;

		math::Vector2 half(0.5f, 0.5f);

		game::Entity* entity = GetOwner();
		for (uint32_t childIndex = 0; childIndex < entity->GetChildren().Size(); ++childIndex)
		{
			Node* childNode = entity->GetChildren()[childIndex].Lock()->GetComponent<Node>();
			if (childNode != nullptr)
			{
				if (childNode->_anchorMin != half || childNode->_anchorMax != half || childNode->_pivot != half)
				{
					childNode->MarkSizeAsDirty();
					childNode->MarkLocalMatrixAsDirty();
				}
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
			game::Entity* entity = GetOwner();
			for (uint32_t childIndex = 0; childIndex < entity->GetChildren().Size(); ++childIndex)
			{
				Node* childNode = entity->GetChildren()[childIndex].Lock()->GetComponent<Node>();
				if (childNode != nullptr)
				{
					childNode->MarkCanvasMatrixAsDirty();
				}
			}
		}
	}

	/// @brief
	/// @return
	const math::Vector2& Node::ComputeSize()
	{
		if (_dirtyFlags & (uint8_t)DirtyFlag::Size)
		{
			Node* pParent = _parent.Get();

			math::Vector2 previousSize = _size;
			ComputeSize(pParent, _size);

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
	void Node::ComputeSize(Node* pParent, math::Vector2& rSize)
	{
		math::Vector2 deltaAnchor = _anchorMax - _anchorMin;

		if (deltaAnchor != math::Vector2::Zero)
		{
			math::Vector2 vParentSize = math::Vector2::Zero;
			if (pParent != nullptr)
			{
				vParentSize = pParent->ComputeSize();
			}

			rSize = (vParentSize * deltaAnchor) + _deltaSize;
		}
		else
		{
			rSize = _deltaSize;
		}
	}

	/// @brief
	/// @return
	const math::Matrix4& Node::ComputeLocalMatrix()
	{
		ComputeSize();
		if (_dirtyFlags & (uint8_t)DirtyFlag::LocalMatrix)
		{
			ComputeLocalMatrix(_parent.Get(), _size, _localMatrix);

			_dirtyFlags &= ~((uint8_t)DirtyFlag::LocalMatrix);
		}

		return _localMatrix;
	}

	/// @brief
	/// @return
	void Node::ComputeLocalMatrix(Node* parent, const math::Vector2& Size, math::Matrix4& localMatrix) const
	{
		math::Vector2 position(0.0f, 0.0f);

		math::Vector2 parentSize = math::Vector2::Zero;
		if (parent != nullptr)
		{
			parentSize = parent->ComputeSize();
		}

		const math::Vector2& anchorSize = _anchorMax - _anchorMin;
		const math::Vector2& anchorPos = (_anchorMin + _anchorMax) * 0.5f;

		position += (anchorPos - math::Vector2(0.5f, 0.5f)) * parentSize;
		position += (_pivot - math::Vector2(0.5f, 0.5f)) * parentSize * anchorSize;

		position += GetPosition();

		const math::Vector2& pixelPivot = (_pivot - math::Vector2(0.5f, 0.5f)) * Size;

		math::Matrix4 anchor = math::Matrix4::Translation(pixelPivot);
		math::Matrix4 inverseAnchor = math::Matrix4::Translation(-pixelPivot);

		math::Matrix4 translation = math::Matrix4::Translation(position);
		math::Matrix4 rotation = math::Matrix4::Rotation(_rotation);
		math::Matrix4 scaling = math::Matrix4::Scale(_scale);

		localMatrix = inverseAnchor * scaling * rotation * anchor * translation * inverseAnchor;
	}

	/// @brief
	/// @return
	const math::Matrix4& Node::ComputeCanvasMatrix()
	{
		ComputeLocalMatrix();
		if (_dirtyFlags & (uint8_t)DirtyFlag::CanvasMatrix)
		{
			Node* parent = _parent.Get();

			if (parent != nullptr)
			{
				_canvasMatrix = _localMatrix * parent->ComputeCanvasMatrix();
			}
			else
			{
				_canvasMatrix = _localMatrix;
			}

			_dirtyFlags &= ~((uint8_t)DirtyFlag::CanvasMatrix);
		}

		return _canvasMatrix;
	}

	math::Matrix4 Node::ComputeWorldMatrix()
	{
		math::Matrix4 worldMatrix;
		Canvas* canvas = GetOwner()->GetComponentInParent<Canvas>();
		if (canvas != nullptr)
		{
			worldMatrix = canvas->GetRenderModeMatrix() * ComputeCanvasMatrix();
		}
		return worldMatrix;
	}

	/// @brief
	/// @return
	Node* Node::GetParent() const
	{
		return _parent.Get();
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
}

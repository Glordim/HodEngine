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
	const Vector2& Node::GetPosition() const
	{
		return _position;
	}

	/// @brief
	/// @param position
	void Node::SetPosition(const Vector2& position)
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
	const Vector2& Node::GetAnchorMin() const
	{
		return _anchorMin;
	}

	/// @brief
	/// @param anchorMin
	void Node::SetAnchorMin(const Vector2& anchorMin)
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
	const Vector2& Node::GetAnchorMax() const
	{
		return _anchorMax;
	}

	/// @brief
	/// @param anchorMax
	void Node::SetAnchorMax(const Vector2& anchorMax)
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
	const Vector2& Node::GetPivot() const
	{
		return _pivot;
	}

	/// @brief
	/// @param pivot
	void Node::SetPivot(const Vector2& pivot)
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
	const Vector2& Node::GetDeltaSize() const
	{
		return _deltaSize;
	}

	/// @brief
	/// @param deltaSize
	void Node::SetDeltaSize(const Vector2& deltaSize)
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

		Vector2 half(0.5f, 0.5f);

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
	const Vector2& Node::ComputeSize()
	{
		if (_dirtyFlags & (uint8_t)DirtyFlag::Size)
		{
			Node* pParent = _parent.Get();

			Vector2 previousSize = _size;
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
	void Node::ComputeSize(Node* pParent, Vector2& rSize)
	{
		Vector2 deltaAnchor = _anchorMax - _anchorMin;

		if (deltaAnchor != Vector2::Zero)
		{
			Vector2 vParentSize = Vector2::Zero;
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
	const Matrix4& Node::ComputeLocalMatrix()
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
	void Node::ComputeLocalMatrix(Node* parent, const Vector2& Size, Matrix4& localMatrix) const
	{
		Vector2 position(0.0f, 0.0f);

		Vector2 parentSize = Vector2::Zero;
		if (parent != nullptr)
		{
			parentSize = parent->ComputeSize();
		}

		const Vector2& anchorSize = _anchorMax - _anchorMin;
		const Vector2& anchorPos = (_anchorMin + _anchorMax) * 0.5f;

		position += (anchorPos - Vector2(0.5f, 0.5f)) * parentSize;
		position += (_pivot - Vector2(0.5f, 0.5f)) * parentSize * anchorSize;

		position += GetPosition();

		const Vector2& pixelPivot = (_pivot - Vector2(0.5f, 0.5f)) * Size;

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

	Matrix4 Node::ComputeWorldMatrix()
	{
		Matrix4 worldMatrix;
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

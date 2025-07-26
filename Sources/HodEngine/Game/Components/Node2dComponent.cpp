#include "HodEngine/Game/Pch.hpp"
#include "Node2dComponent.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#include "HodEngine/Core/Math/Math.hpp"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(ZOrder, reflectionDescriptor)
		{
			reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
			[](const void* instance, Document::Node& documentNode)
			{
				const ZOrder* zorder = static_cast<const ZOrder*>(instance);
				documentNode.AddChild("Layer").SetUInt16(zorder->GetLayer());
				documentNode.AddChild("InternalOrder").SetInt16(zorder->GetInternalOrder());
				return true;
			},
			[](void* instance, const Document::Node& documentNode)
			{
				ZOrder* zorder = static_cast<ZOrder*>(instance);
				const Document::Node* layerNode = documentNode.GetChild("Layer");
				if (layerNode)
				{
					zorder->SetLayer(layerNode->GetUInt16());
				}
				const Document::Node* internalOrderNode = documentNode.GetChild("InternalOrder");
				if (internalOrderNode)
				{
					zorder->SetInternalOrder(internalOrderNode->GetInt16());
				}
				return true;
			});
		}

		ZOrder::ZOrder(uint16_t layer, uint16_t internalOrder)
		{
			_unifiedValue.split._layer = layer;
			_unifiedValue.split._internalOrder = internalOrder;
		}

		void ZOrder::SetLayer(uint16_t layer)
		{
			_unifiedValue.split._layer = layer;
		}

		uint16_t ZOrder::GetLayer() const
		{
			return _unifiedValue.split._layer;
		}

		void ZOrder::SetInternalOrder(int16_t internalOrder)
		{
			_unifiedValue.split._internalOrder = internalOrder;
		}

		int16_t ZOrder::GetInternalOrder() const
		{
			return _unifiedValue.split._internalOrder;
		}

		uint32_t ZOrder::GetValue() const
		{
			uint16_t order = static_cast<uint16_t>(_unifiedValue.split._internalOrder ^ 0x8000);
			return (static_cast<uint32_t>(_unifiedValue.split._layer) << 16) | order;
		}

		DESCRIBE_REFLECTED_CLASS(Node2dComponent, reflectionDescriptor)
		{
			AddPropertyT(reflectionDescriptor, &Node2dComponent::_position, "_position", &Node2dComponent::SetPosition);
			AddPropertyT(reflectionDescriptor, &Node2dComponent::_rotation, "_rotation", &Node2dComponent::SetRotation);
			AddPropertyT(reflectionDescriptor, &Node2dComponent::_scale, "_scale", &Node2dComponent::SetScale);

			AddPropertyT(reflectionDescriptor, &Node2dComponent::_zOrder, "_zOrder");
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::ComputeLocalMatrix(Matrix4& localMatrix)
		{
			localMatrix = Matrix4::Translation(_position) * Matrix4::Rotation(math::DegreeToRadian(_rotation)) * Matrix4::Scale(_scale);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::SetPosition(const Vector2& position)
		{
			_position = position;
			SetLocalMatrixDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Vector2& Node2dComponent::GetPosition() const
		{
			return _position;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::Rotate(float angle)
		{
			_rotation += angle;
			SetLocalMatrixDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::SetRotation(float angle)
		{
			_rotation = angle;
			SetLocalMatrixDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		float Node2dComponent::GetRotation() const
		{
			return _rotation;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::SetScale(const Vector2& scale)
		{
			_scale = scale;
			SetLocalMatrixDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const Vector2& Node2dComponent::GetScale() const
		{
			return _scale;
		}

		void Node2dComponent::SetZOrder(ZOrder zOrder)
		{
			_zOrder = zOrder;
		}

		ZOrder Node2dComponent::GetZOrder() const
		{
			return _zOrder;
		}
	}
}

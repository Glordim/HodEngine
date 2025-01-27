#include "HodEngine/Game/Pch.hpp"
#include "Node2dComponent.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.hpp>

#define _USE_MATH_DEFINES
#include <cmath>

#include "HodEngine/Core/Math/Math.hpp"

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(Node2dComponent, reflectionDescriptor)
		{
			AddPropertyT(reflectionDescriptor, &Node2dComponent::_position, "_position", &Node2dComponent::SetPosition);
			AddPropertyT(reflectionDescriptor, &Node2dComponent::_rotation, "_rotation", &Node2dComponent::SetRotation);
			AddPropertyT(reflectionDescriptor, &Node2dComponent::_scale, "_scale", &Node2dComponent::SetScale);
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
	}
}

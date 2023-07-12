#include "Node2dComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitFixedSizeArray.h>
#include <HodEngine/Game/Components/Node2dComponentCustomEditor.h>

#define _USE_MATH_DEFINES
#include <cmath>

constexpr float pi = 3.14159265358979323846f;

namespace hod
{
	using namespace Reflection::Property;

	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(Node2dComponent, NodeComponent)
		{
			ADD_PROPERTY_WITH_SET_METHOD(Node2dComponent, _position, SetPosition); // todo << ReflectionTraitFixedSizeArray(2);
			ADD_PROPERTY_WITH_SET_METHOD(Node2dComponent, _rotation, SetRotation);
			ADD_PROPERTY_WITH_SET_METHOD(Node2dComponent, _scale, SetScale);
			
			ADD_CUSTOM_EDITOR(Node2dComponentCustomEditor);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* Node2dComponent::GetType() const
		{
			return "Node2d";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::ComputeLocalMatrix(Matrix4& localMatrix)
		{
			localMatrix = Matrix4::Translation(_position) * Matrix4::Rotation(_rotation * (pi / 180)) * Matrix4::Scale(_scale);
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

#include "Node2dComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyVariable.h>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyArray.h>
#include <HodEngine/Core/Reflection/Traits/ReflectionTraitFixedSizeArray.h>

namespace hod
{
	using namespace core::Reflection::Property;

	namespace game
	{
		DECLARE_HOD_COMPONENT(Node2dComponent, NodeComponent)

		DESCRIBE_REFLECTED_DERIVED_CLASS(Node2dComponent, NodeComponent)
		{
			AddProperty<Array>(Variable::Type::Float32, offsetof(Node2dComponent, _position), "Position")->AddTrait<core::ReflectionTraitFixedSizeArray>(2);
			AddProperty<Variable>(Variable::Type::Float32, offsetof(Node2dComponent, _rotation), "Rotation");
			AddProperty<Array>(Variable::Type::Float32, offsetof(Node2dComponent, _scale), "Scale")->AddTrait<core::ReflectionTraitFixedSizeArray>(2);
		}

		/// @brief 
		/// @param entity 
		Node2dComponent::Node2dComponent(const std::weak_ptr<Entity>& entity)
		: NodeComponent(entity)
		{
			_position = glm::vec2(0.0f, 0.0f);
			_rotation = 0.0f;
			_scale = glm::vec2(1.0f, 1.0f);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Node2dComponent::~Node2dComponent()
		{

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
		void Node2dComponent::ComputeLocalMatrix(glm::mat4& localMatrix)
		{
			glm::mat4 pos = glm::translate(glm::identity<glm::mat4>(), glm::vec3(_position, 0.0f));
			glm::quat rot = glm::rotate(glm::identity<glm::quat>(), glm::radians<float>(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3(_scale, 1.0f));

			localMatrix = pos * glm::mat4_cast(rot) * scale;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Node2dComponent::SetPosition(const glm::vec2& position)
		{
			_position = position;
			SetLocalMatrixDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec2& Node2dComponent::GetPosition() const
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
		void Node2dComponent::SetScale(const glm::vec2& scale)
		{
			_scale = scale;
			SetLocalMatrixDirty();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec2& Node2dComponent::GetScale() const
		{
			return _scale;
		}
	}
}

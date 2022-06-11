#include "SceneComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <iostream>
#include <string>

#include "../Actor.h"
#include <HodEngine/Physics/src/Actor.h>

#include <HodEngine/ImGui/src/imgui.h>

#include <HodEngine/Core/Src/Reflection/Properties/ReflectionPropertyVariable.h>

namespace HOD
{
	using namespace CORE::Reflection::Property;

	namespace GAME
	{
		DESCRIBE_REFLECTED_DERIVED_CLASS(SceneComponent, Component)
		{
			AddProperty<Variable>(Variable::Type::Vector2f32, offsetof(SceneComponent, _position), "Position");
			AddProperty<Variable>(Variable::Type::Float32, offsetof(SceneComponent, _rotation), "Rotation");
			AddProperty<Variable>(Variable::Type::Vector2f32, offsetof(SceneComponent, _scale), "Scale");
		}

		/// @brief 
		/// @param actor 
		SceneComponent::SceneComponent(Actor* actor) : Component(actor)
		{
			_position = glm::vec2(0.0f, 0.0f);
			_rotation = 0.0f;
			_scale = glm::vec2(1.0f, 1.0f);
			_parent = nullptr;

			_modelMatrixDirty = true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SceneComponent::~SceneComponent()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::DrawImGui()
		{
			glm::vec2 pos = _position;

			if (ImGui::DragFloat2("Position", &pos[0]) == true)
			{
				SetPosition(pos);
			}

			float rotation = _rotation;

			if (ImGui::DragFloat("Rotation", &rotation) == true)
			{
				SetRotation(rotation);
			}

			glm::vec2 scale = _scale;

			if (ImGui::DragFloat2("Scale", &scale[0]) == true)
			{
				SetScale(scale);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const char* SceneComponent::GetType() const
		{
			return "Scene";
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::mat4& SceneComponent::GetModelMatrix()
		{
			if (_modelMatrixDirty == true)
			{
				glm::mat4 pos = glm::translate(glm::identity<glm::mat4>(), glm::vec3(_position, 0.0f));
				glm::quat rot = glm::rotate(glm::identity<glm::quat>(), glm::radians<float>(_rotation), glm::vec3(0.0f, 0.0f, 1.0f));
				glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), glm::vec3(_scale, 1.0f));

				_modelMatrix = pos * glm::mat4_cast(rot) * scale;
			}

			return _modelMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetPosition(const glm::vec2& position)
		{
			_position = position;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec2& SceneComponent::GetPosition() const
		{
			return _position;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::Rotate(float angle)
		{
			_rotation += angle;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetRotation(float angle)
		{
			_rotation = angle;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		float SceneComponent::GetRotation() const
		{
			return _rotation;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetScale(const glm::vec2& scale)
		{
			_scale = scale;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::vec2& SceneComponent::GetScale() const
		{
			return _scale;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint32_t SceneComponent::GetChildCount() const
		{
			return static_cast<uint32_t>(_childs.size());
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SceneComponent* SceneComponent::GetChild(uint32_t index)
		{
			return _childs[index];
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetParent(SceneComponent* parent)
		{
			// TODO retirer du vector childs du precedent parent

			_parent = parent;
			_parent->_childs.push_back(this);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SyncPxActor()
		{
			PHYSICS::Actor* physicActor = GetActor()->GetPhysicActor();

			if (physicActor != nullptr)
			{
				//physicActor->SetTransform(_position, _rotation, _scale);
			}
		}
	}
}

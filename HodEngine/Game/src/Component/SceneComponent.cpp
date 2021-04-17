#include "SceneComponent.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_decompose.hpp"

#include <iostream>
#include <string>

//#include <AntTweakBar.h>

#include "../Actor.h"
#include <Physics/src/Actor.h>

#include <ImGui/src/imgui.h>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
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

			if (ImGui::DragFloat3("Position", &pos[0]) == true)
			{
				SetPosition(pos);
			}

			float rotation = _rotation;

			if (ImGui::DragFloat("Rotation", &rotation) == true)
			{
				SetRotation(rotation);
			}

			glm::vec2 scale = _scale;

			if (ImGui::DragFloat3("Scale", &scale[0]) == true)
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
		void SceneComponent::LookAt(const glm::vec3& eye, const glm::vec3 target, const glm::vec3 up)
		{
			// TODO
			/*
			_modelMatrix = glm::lookAt(eye, target, up);

			glm::vec3 skew;
			glm::vec4 perspective;

			glm::decompose(_modelMatrix, _scale, _rotation, _position, skew, perspective);
			_rotation = glm::conjugate(_rotation);

			_modelMatrixDirty = false;

			std::cout << std::to_string(_position.x) << " " << std::to_string(_position.y) << " " << std::to_string(_position.z) << std::endl;
			std::cout << std::to_string(GetRotationEuler().x) << " " << std::to_string(GetRotationEuler().y) << " " << std::to_string(GetRotationEuler().z) << std::endl;
			*/
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		const glm::mat4& SceneComponent::GetModelMatrix()
		{
			if (_modelMatrixDirty == true)
			{
				/*
				glm::mat4 pos = glm::translate(glm::identity<glm::mat4>(), _position);
				glm::mat4 rot = glm::mat4_cast(_rotation);
				glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), _scale);

				_modelMatrix = pos * rot * scale;
				*/

				// TODO
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
				physicActor->SetTransform(_position, _rotation, _scale);
			}
		}
	}
}

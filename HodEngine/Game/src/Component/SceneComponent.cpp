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
			_position = glm::vec3(0.0f, 0.0f, 0.0f);
			_rotation = glm::identity<glm::mat4x4>();
			_scale = glm::vec3(1.0f, 1.0f, 1.0f);
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
			glm::vec3 pos = _position;

			if (ImGui::DragFloat3("Position", &pos[0]) == true)
			{
				SetPosition(pos);
			}

			glm::vec3 euler = glm::degrees(glm::eulerAngles(_rotation));

			if (ImGui::DragFloat3("Rotation", &euler[0]) == true)
			{
				SetRotation(euler);
			}

			glm::vec3 scale = _scale;

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
		void SceneComponent::twGetPos(void* value, void* clientData)
		{
			SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
			glm::vec3* pos = static_cast<glm::vec3*>(value);
			*pos = thiz->GetPosition();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::twSetPos(const void* value, void* clientData)
		{
			SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
			thiz->SetPosition(*(static_cast<const glm::vec3*>(value)));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::twGetRot(void* value, void* clientData)
		{
			SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
			glm::quat* pos = static_cast<glm::quat*>(value);
			*pos = thiz->GetRotation();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::twSetRot(const void* value, void* clientData)
		{
			SceneComponent* thiz = static_cast<SceneComponent*>(clientData);
			thiz->SetRotation(*(static_cast<const glm::quat*>(value)));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::LookAt(const glm::vec3& eye, const glm::vec3 target, const glm::vec3 up)
		{
			_modelMatrix = glm::lookAt(eye, target, up);

			glm::vec3 skew;
			glm::vec4 perspective;

			glm::decompose(_modelMatrix, _scale, _rotation, _position, skew, perspective);
			_rotation = glm::conjugate(_rotation);

			_modelMatrixDirty = false;

			std::cout << std::to_string(_position.x) << " " << std::to_string(_position.y) << " " << std::to_string(_position.z) << std::endl;
			std::cout << std::to_string(GetRotationEuler().x) << " " << std::to_string(GetRotationEuler().y) << " " << std::to_string(GetRotationEuler().z) << std::endl;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		glm::mat4 SceneComponent::GetModelMatrix()
		{
			if (_modelMatrixDirty == true)
			{
				glm::mat4 pos = glm::translate(glm::identity<glm::mat4>(), _position);
				glm::mat4 rot = glm::mat4_cast(_rotation);
				glm::mat4 scale = glm::scale(glm::identity<glm::mat4>(), _scale);

				_modelMatrix = pos * rot * scale;
			}

			return _modelMatrix;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetPosition(glm::vec3 position)
		{
			_position = position;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		glm::vec3 SceneComponent::GetPosition() const
		{
			return _position;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::Rotate(float angle, glm::vec3 axis)
		{
			_rotation = glm::rotate(_rotation, angle, axis);
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetRotation(glm::quat rot)
		{
			_rotation = rot;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetRotation(glm::vec3 rot)
		{
			rot.x = fmod(rot.x, 360.0f);
			rot.y = fmod(rot.y, 360.0f);
			rot.z = fmod(rot.z, 360.0f);

			_rotation = glm::quat(glm::radians(rot));
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		glm::quat SceneComponent::GetRotation() const
		{
			return _rotation;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		glm::vec3 SceneComponent::GetRotationEuler() const
		{
			return glm::degrees(glm::eulerAngles(_rotation));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void SceneComponent::SetScale(glm::vec3 scale)
		{
			_scale = scale;
			_modelMatrixDirty = true;

			SyncPxActor();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		glm::vec3 SceneComponent::GetScale() const
		{
			return _scale;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		size_t SceneComponent::GetChildCount() const
		{
			return _childs.size();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		SceneComponent* SceneComponent::GetChild(size_t index)
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

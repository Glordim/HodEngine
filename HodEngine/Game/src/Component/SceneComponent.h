#pragma once

#include "../Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace HOD
{
	namespace GAME
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class SceneComponent : public Component
		{
		public:

											SceneComponent(Actor* actor);
											SceneComponent(const SceneComponent&) = delete;
											SceneComponent(SceneComponent&&) = delete;
											~SceneComponent() override;

			void							operator=(const SceneComponent&) = delete;
			void							operator=(SceneComponent&&) = delete;

		public:

			void							DrawImGui() override;
			const char*						GetType() const override;

		public:

			size_t							GetChildCount() const;
			SceneComponent*					GetChild(size_t index);
			void							SetParent(SceneComponent* parent);

			void							LookAt(const glm::vec3& eye, const glm::vec3 target, const glm::vec3 up);

			glm::mat4						GetModelMatrix();

			void							SetPosition(glm::vec3 position);
			glm::vec3						GetPosition() const;

			void							SetRotation(glm::quat rot);
			void							SetRotation(glm::vec3 rotation);
			glm::quat						GetRotation() const;
			glm::vec3						GetRotationEuler() const;
			void							Rotate(float angle, glm::vec3 axis);

			void							SetScale(glm::vec3 scale);
			glm::vec3						GetScale() const;

		public:

			glm::vec3						_position;
			glm::vec3						_scale;

		private:

			void							SyncPxActor();

		private:

			bool							_modelMatrixDirty;
			glm::mat4						_modelMatrix;

			glm::quat						_rotation;

			std::vector<SceneComponent*>	_childs;
			SceneComponent*					_parent;

		private:

			static void						twSetPos(const void* value, void* clientData);
			static void						twGetPos(void* value, void* clientData);

			static void						twSetRot(const void* value, void* clientData);
			static void						twGetRot(void* value, void* clientData);
		};
	}
}

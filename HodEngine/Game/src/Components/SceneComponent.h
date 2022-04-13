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
			//REFLECTED_DERIVED_CLASS(SceneComponent, Component);

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

			uint32_t						GetChildCount() const;
			SceneComponent*					GetChild(uint32_t index);
			void							SetParent(SceneComponent* parent);

			const glm::mat4&				GetModelMatrix();

			void							SetPosition(const glm::vec2& position);
			const glm::vec2&				GetPosition() const;

			void							SetRotation(float rot);
			float							GetRotation() const;
			void							Rotate(float angle);

			void							SetScale(const glm::vec2& scale);
			const glm::vec2&				GetScale() const;

		private:

			void							SyncPxActor();

		private:

			bool							_modelMatrixDirty;
			glm::mat4						_modelMatrix;

			glm::vec2						_position;
			glm::vec2						_scale;
			float							_rotation;

			std::vector<SceneComponent*>	_childs;
			SceneComponent*					_parent;
		};
	}
}

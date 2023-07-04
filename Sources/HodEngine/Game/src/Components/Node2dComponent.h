#pragma once

#include "HodEngine/Game/src/Components/NodeComponent.h"
#include "HodEngine/Core/Math/Vector2.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

namespace hod
{
	namespace game
	{
		/// @brief 
		class Node2dComponent : public NodeComponent
		{
			HOD_COMPONENT(Node2dComponent, NodeComponent)
			//REFLECTED_DERIVED_CLASS(Node2dComponent, NodeComponent)
			//META_TYPE(Node2dComponent)

		public:

											Node2dComponent(const std::weak_ptr<Entity>& entity);
											Node2dComponent(const Node2dComponent&) = delete;
											Node2dComponent(Node2dComponent&&) = delete;
											~Node2dComponent() override;

			void							operator=(const Node2dComponent&) = delete;
			void							operator=(Node2dComponent&&) = delete;

		public:

			const char*						GetType() const override;

			void							SetPosition(const Vector2& position);
			const Vector2&					GetPosition() const;

			void							SetRotation(float rot);
			float							GetRotation() const;
			void							Rotate(float angle);

			void							SetScale(const Vector2& scale);
			const Vector2&					GetScale() const;

		protected:

			void							ComputeLocalMatrix(glm::mat4& localMatrix) override;

		private:

			Vector2							_position;
			Vector2							_scale;
			float							_rotation;
		};
	}
}

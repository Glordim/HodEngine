#pragma once

#include "../Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>

namespace hod
{
	namespace game
	{
		/// @brief 
		class NodeComponent : public Component
		{
			REFLECTED_DERIVED_CLASS(NodeComponent, Component);

		public:

											NodeComponent(Actor* actor);
											NodeComponent(const NodeComponent&) = delete;
											NodeComponent(NodeComponent&&) = delete;
											~NodeComponent() override;

			void							operator=(const NodeComponent&) = delete;
			void							operator=(NodeComponent&&) = delete;

		public:

			const char*						GetType() const override;

		public:

			uint32_t						GetChildCount() const;
			NodeComponent*					GetChild(uint32_t index);
			void							SetParent(NodeComponent* parent);

			const glm::mat4&				GetLocalMatrix();
			const glm::mat4&				GetWorldMatrix();

		protected:

			virtual void					ComputeLocalMatrix(glm::mat4& localMatrix) = 0;
			void							SetLocalMatrixDirty();

		private:

			void							ComputeWorldMatrix(const glm::mat4& parentMatrix);

		private:

			bool							_localMatrixDirty = true;
			glm::mat4						_localMatrix;
			glm::mat4						_worldMatrix;

			std::vector<NodeComponent*>		_childs;
			NodeComponent*					_parent = nullptr;
		};
	}
}

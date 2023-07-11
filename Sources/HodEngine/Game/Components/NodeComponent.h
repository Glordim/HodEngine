#pragma once

#include "HodEngine/Game/Component.h"

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "HodEngine/Core/Math/Matrix4.h"

#include <vector>
#include <memory>

namespace hod
{
	namespace game
	{
		/// @brief 
		class NodeComponent : public Component
		{
			REFLECTED_CLASS(NodeComponent, Component)

		public:

											NodeComponent() = default;
											NodeComponent(const NodeComponent&) = delete;
											NodeComponent(NodeComponent&&) = delete;
											~NodeComponent() override;

			void							operator=(const NodeComponent&) = delete;
			void							operator=(NodeComponent&&) = delete;

		public:

			const char*						GetType() const override;

		public:

			uint32_t						GetChildCount() const;
			std::weak_ptr<NodeComponent>	GetChild(uint32_t index);

			std::weak_ptr<NodeComponent>	GetParent() const;
			void							SetParent(const std::weak_ptr<NodeComponent>& parent);

			const Matrix4&					GetLocalMatrix();
			const Matrix4&					GetWorldMatrix() { return _worldMatrix; }

		protected:

			virtual void					ComputeLocalMatrix(Matrix4& localMatrix) { localMatrix = Matrix4::Identity; }
			void							SetLocalMatrixDirty();

		private:

			void							ComputeWorldMatrix(const glm::mat4& parentMatrix);

		private:

			bool										_localMatrixDirty = true;
			Matrix4										_localMatrix = Matrix4::Identity;
			Matrix4										_worldMatrix = Matrix4::Identity;

			std::vector<std::weak_ptr<NodeComponent>>	_children;
			std::weak_ptr<NodeComponent>				_parent;
		};
	}
}

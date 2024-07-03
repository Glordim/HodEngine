#pragma once
#include <HodEngine/HodEngine.hpp>

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Core/Math/Matrix4.hpp"

#include <vector>
#include <memory>

namespace hod
{
	namespace game
	{
		/// @brief 
		class HODENGINE_API NodeComponent : public Component
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

			uint32_t							GetChildCount() const;
			const WeakComponent<NodeComponent>&	GetChild(uint32_t index);

			const WeakComponent<NodeComponent>&	GetParent() const;
			void								SetParent(const WeakComponent<NodeComponent>& parent);

			const Matrix4&						GetLocalMatrix();
			Matrix4								GetWorldMatrix();

		protected:

			virtual void						ComputeLocalMatrix(Matrix4& localMatrix) { localMatrix = Matrix4::Identity; }
			void								SetLocalMatrixDirty();

			void								OnConstruct() override;

		private:

			void								ComputeWorldMatrix(const Matrix4& parentMatrix);

		private:

			bool										_localMatrixDirty = true;
			Matrix4										_localMatrix = Matrix4::Identity;
			Matrix4										_worldMatrix = Matrix4::Identity;

			std::vector<WeakComponent<NodeComponent>>	_children;
			WeakComponent<NodeComponent>				_parent;
		};
	}
}

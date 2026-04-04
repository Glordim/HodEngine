#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Math/Matrix4.hpp"

#include "HodEngine/Core/Vector.hpp"
#include <memory>

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API NodeComponent : public Component
	{
		REFLECTED_CLASS(NodeComponent, Component)

	public:

								NodeComponent() = default;
								NodeComponent(const NodeComponent&) = delete;
								NodeComponent(NodeComponent&&) = delete;
								~NodeComponent() override;

		void					operator=(const NodeComponent&) = delete;
		void					operator=(NodeComponent&&) = delete;

	public:

		const math::Matrix4&			GetLocalMatrix();
		math::Matrix4					GetWorldMatrix();

	protected:

		virtual void			ComputeLocalMatrix(math::Matrix4& localMatrix) { localMatrix = math::Matrix4::Identity; }
		void					SetLocalMatrixDirty();

	private:

		void					ComputeWorldMatrix(const math::Matrix4& parentMatrix);

	private:

		bool					_localMatrixDirty = true;
		math::Matrix4					_localMatrix = math::Matrix4::Identity;
		math::Matrix4					_worldMatrix = math::Matrix4::Identity;
	};
}

#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"
#include "HodEngine/Game/WeakComponent.hpp"

#include "HodEngine/Core/Math/Matrix4.hpp"

#include <vector>
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

		const Matrix4&			GetLocalMatrix();
		Matrix4					GetWorldMatrix();

	protected:

		virtual void			ComputeLocalMatrix(Matrix4& localMatrix) { localMatrix = Matrix4::Identity; }
		void					SetLocalMatrixDirty();

	private:

		void					ComputeWorldMatrix(const Matrix4& parentMatrix);

	private:

		bool					_localMatrixDirty = true;
		Matrix4					_localMatrix = Matrix4::Identity;
		Matrix4					_worldMatrix = Matrix4::Identity;
	};
}

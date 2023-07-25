#pragma once

#include "HodEngine/Game/Components/NodeComponent.h"
#include "HodEngine/Core/Math/Vector2.h"

namespace hod
{
	namespace game
	{
		/// @brief 
		class Node2dComponent : public NodeComponent
		{
			REFLECTED_CLASS(Node2dComponent, NodeComponent)

		public:

											Node2dComponent() = default;
											Node2dComponent(const Node2dComponent&) = delete;
											Node2dComponent(Node2dComponent&&) = delete;
											~Node2dComponent() override = default;

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

			void							ComputeLocalMatrix(Matrix4& localMatrix) override;

		private:

			Vector2							_position = Vector2::Zero;
			Vector2							_scale = Vector2::One;
			float							_rotation = 0.0f;
		};
	}
}

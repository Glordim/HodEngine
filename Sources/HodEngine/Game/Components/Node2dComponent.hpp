#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/NodeComponent.hpp"
#include "HodEngine/Core/Math/Vector2.hpp"

namespace hod
{
	namespace game
	{
		class HOD_GAME_API ZOrder
		{
			REFLECTED_CLASS_NO_VIRTUAL(ZOrder)

		public:

			ZOrder(uint16_t layer, uint16_t internalOrder);

			void		SetLayer(uint16_t layer);
			uint16_t	GetLayer() const;

			void		SetInternalOrder(int16_t internalOrder);
			int16_t		GetInternalOrder() const;

			uint32_t	GetValue() const;

		private:

			union UnfiedValue
			{
				struct
				{
					int16_t		_internalOrder;
					uint16_t	_layer;
				};
				uint32_t _value = 0;
			} _unifiedValue;
		};

		/// @brief 
		class HOD_GAME_API Node2dComponent : public NodeComponent
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

			void							SetPosition(const Vector2& position);
			const Vector2&					GetPosition() const;

			void							SetRotation(float rot);
			float							GetRotation() const;
			void							Rotate(float angle);

			void							SetScale(const Vector2& scale);
			const Vector2&					GetScale() const;

			void							SetZOrder(ZOrder zOrder);
			ZOrder							GetZOrder() const;

		protected:

			void							ComputeLocalMatrix(Matrix4& localMatrix) override;

		private:

			Vector2							_position = Vector2::Zero;
			Vector2							_scale = Vector2::One;
			float							_rotation = 0.0f;

			ZOrder							_zOrder = ZOrder(0, 0);
		};
	}
}

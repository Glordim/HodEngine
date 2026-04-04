#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Math/Quaternion.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Core/Event.hpp>

namespace hod::game
{
	class Entity;
}

namespace hod::ui
{
	/// @brief 
	class HOD_UI_API Node : public game::Component
	{
		REFLECTED_CLASS(Node, game::Component)

	public:

		using PropertyChangedEvent = Event<>;

	public:

		const math::Vector2&				GetPosition() const;
		void						SetPosition(const math::Vector2& position);

		float						GetRotation() const;
		void						SetRotation(float rotation);
		//void						SetWorldRotation(float rotation);

		const math::Vector2&				GetScale() const;
		void						SetScale(const math::Vector2& scale);

		const math::Vector2&				GetAnchorMin() const;
		void						SetAnchorMin(const math::Vector2& anchorMin);

		const math::Vector2&				GetAnchorMax() const;
		void						SetAnchorMax(const math::Vector2& anchorMax);

		const math::Vector2&				GetPivot() const;
		void						SetPivot(const math::Vector2& pivot);

		const math::Vector2&				GetDeltaSize() const;
		void						SetDeltaSize(const math::Vector2& deltaSize);

		void						SetZOrder(int32_t zOrder);
		int32_t						GetZOrder() const;
		int32_t						ComputeZOrder();

		void						MarkSizeAsDirty();
		void						MarkZOrderAsDirty();
		void						MarkLocalMatrixAsDirty();
		void						MarkCanvasMatrixAsDirty();
		void						MarkCanvasMatrixAsDirtyOnAllSubGameObject(const game::Entity* gameObject);

		const math::Vector2&				ComputeSize();
		void						ComputeSize(Node* parent, math::Vector2& size);
		const math::Matrix4&				ComputeLocalMatrix();
		void						ComputeLocalMatrix(Node* parent, const math::Vector2& size, math::Matrix4& localMatrix) const;
		const math::Matrix4&				ComputeCanvasMatrix();
		math::Matrix4						ComputeWorldMatrix();
		Node*						GetParent() const;

		void						OnEnable() override;

		PropertyChangedEvent&		GetPropertyChangedEvent();

	private:

		enum class DirtyFlag : uint8_t
		{
			LocalMatrix = (1 << 0),
			CanvasMatrix = (1 << 1),
			Size = (1 << 2),
			ZOrder = (1 << 4),
		};

	private:

		int32_t						_zOrder = std::numeric_limits<int32_t>().lowest();
		int32_t						_globalZOrder = 0;

		math::Vector2						_position = math::Vector2::Zero;
		float						_rotation = 0.0f;
		math::Vector2						_scale = math::Vector2::One;

		math::Vector2						_anchorMin = math::Vector2(0.5f, 0.5f);
		math::Vector2						_anchorMax = math::Vector2(0.5f, 0.5f);
		math::Vector2						_pivot = math::Vector2(0.5f, 0.5f);
		math::Vector2						_deltaSize = math::Vector2(100.0f, 100.0f);

		math::Vector2						_size = math::Vector2::Zero;
		math::Matrix4						_localMatrix = math::Matrix4::Identity;
		math::Matrix4						_canvasMatrix = math::Matrix4::Identity;
		uint8_t						_dirtyFlags = (uint8_t)DirtyFlag::LocalMatrix | (uint8_t)DirtyFlag::CanvasMatrix | (uint8_t)DirtyFlag::Size | (uint8_t)DirtyFlag::ZOrder;

		WeakPtr<Node>				_parent;

		PropertyChangedEvent		_propertyChangedEvent;
	};
}

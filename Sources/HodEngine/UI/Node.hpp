#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>
#include <HodEngine/Core/Math/Quaternion.hpp>
#include <HodEngine/Core/Math/Matrix4.hpp>
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

		const Vector2&				GetPosition() const;
		void						SetPosition(const Vector2& position);

		float						GetRotation() const;
		void						SetRotation(float rotation);
		//void						SetWorldRotation(float rotation);

		const Vector2&				GetScale() const;
		void						SetScale(const Vector2& scale);

		const Vector2&				GetAnchorMin() const;
		void						SetAnchorMin(const Vector2& anchorMin);

		const Vector2&				GetAnchorMax() const;
		void						SetAnchorMax(const Vector2& anchorMax);

		const Vector2&				GetPivot() const;
		void						SetPivot(const Vector2& pivot);

		const Vector2&				GetDeltaSize() const;
		void						SetDeltaSize(const Vector2& deltaSize);

		void						SetZOrder(int32_t zOrder);
		int32_t						GetZOrder() const;
		int32_t						ComputeZOrder();

		void						MarkSizeAsDirty();
		void						MarkZOrderAsDirty();
		void						MarkLocalMatrixAsDirty();
		void						MarkCanvasMatrixAsDirty();
		void						MarkCanvasMatrixAsDirtyOnAllSubGameObject(const game::Entity* gameObject);

		const Vector2&				ComputeSize();
		void						ComputeSize(Node* parent, Vector2& size);
		const Matrix4&				ComputeLocalMatrix();
		void						ComputeLocalMatrix(Node* parent, const Vector2& size, Matrix4& localMatrix) const;
		const Matrix4&				ComputeCanvasMatrix();
		Node*						GetParent() const;

		void						OnAwake() override;

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

		Vector2						_position;
		float						_rotation;
		Vector2						_scale;

		Vector2						_anchorMin;
		Vector2						_anchorMax;
		Vector2						_pivot;
		Vector2						_deltaSize;

		Vector2						_size;
		Matrix4						_localMatrix = Matrix4::Identity;
		Matrix4						_canvasMatrix = Matrix4::Identity;
		uint8_t						_dirtyFlags = (uint8_t)DirtyFlag::LocalMatrix | (uint8_t)DirtyFlag::CanvasMatrix | (uint8_t)DirtyFlag::Size | (uint8_t)DirtyFlag::ZOrder;

		WeakPtr<Node>				_parent;
	};
}

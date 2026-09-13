#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Math/Quaternion.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Core/Event.hpp>

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline ui2
{
	class LayoutParams;

	/// @brief
	class HOD_UI2_API Node
	{
		REFLECTED_CLASS_NO_PARENT(Node)

	public:

		using PropertyChangedEvent = Event<>;

	public:

		Node() = default;
		virtual ~Node();

		float						GetRotation() const;
		void						SetRotation(float rotation);
		//void						SetWorldRotation(float rotation);

		const Vector2&				GetScale() const;
		void						SetScale(const Vector2& scale);

		// Point of this node's own rect around which its local rotation/scale is applied
		// (akin to CSS transform-origin). Independent of any anchor pivot a LayoutParams may
		// define for positioning: the two can differ on purpose.
		const Vector2&				GetOrigin() const;
		void						SetOrigin(const Vector2& origin);

		const Vector2&				GetDesiredSize() const;
		void						SetDesiredSize(const Vector2& desiredSize);

		void						SetZOrder(int32_t zOrder);
		int32_t						GetZOrder() const;
		int32_t						ComputeZOrder();

		void						MarkSizeAsDirty();
		void						MarkZOrderAsDirty();
		void						MarkLocalMatrixAsDirty();
		void						MarkCanvasMatrixAsDirty();

		const Vector2&				ComputeSize();
		const Matrix4&				ComputeLocalMatrix();
		const Matrix4&				ComputeCanvasMatrix();
		Matrix4						ComputeWorldMatrix();

		// Convenience overload: places child using CreateDefaultLayoutParams(). Prefer the
		// explicit overload below when the child needs a specific, non-default arrangement.
		void						AddChild(Node* child);

		// child must not be null; layoutParams must not be null and must have been freshly
		// allocated for this placement (ownership transfers to this Node, see _layoutParams).
		void						AddChild(Node* child, LayoutParams* layoutParams);
		void						RemoveChild(Node* child);
		Node*						GetParent() const;
		const Vector<Node*>&		GetChildren() const;

		LayoutParams*				GetLayoutParams() const;

		// Factory for the LayoutParams type this node hands to a child added via the
		// no-argument AddChild(). Base Node returns an AnchoredLayoutParams; a Node subclass
		// acting as a specific container (grid, vertical box...) should override this to return
		// its own matching LayoutParams type instead.
		virtual LayoutParams*		CreateDefaultLayoutParams() const;

		PropertyChangedEvent&		GetPropertyChangedEvent();

		// Serializes this node's own scalar properties, its LayoutParams (polymorphic, resolved
		// via LayoutParamsFactory), and its children tree, recursively.
		bool						SerializeInDocument(DocumentNode& documentNode);
		bool						DeserializeFromDocument(const DocumentNode& documentNode);

	private:

		enum class DirtyFlag : uint8_t
		{
			LocalMatrix = (1 << 0),
			CanvasMatrix = (1 << 1),
			Size = (1 << 2),
			ZOrder = (1 << 4),
		};

	private:

		void						ComputeSize(Node* parent, Vector2& size) const;
		void						ComputeLocalMatrix(Node* parent, const Vector2& size, Matrix4& localMatrix) const;

	private:

		int32_t						_zOrder = std::numeric_limits<int32_t>().lowest();
		int32_t						_globalZOrder = 0;

		float						_rotation = 0.0f;
		Vector2						_scale = Vector2::One;
		Vector2						_origin = Vector2(0.5f, 0.5f);

		Vector2						_desiredSize = Vector2(100.0f, 100.0f);

		Vector2						_size = Vector2::Zero;
		Matrix4						_localMatrix = Matrix4::Identity;
		Matrix4						_canvasMatrix = Matrix4::Identity;
		uint8_t						_dirtyFlags = (uint8_t)DirtyFlag::LocalMatrix | (uint8_t)DirtyFlag::CanvasMatrix | (uint8_t)DirtyFlag::Size | (uint8_t)DirtyFlag::ZOrder;

		Node*						_parent = nullptr;

		// Non-owning cache of this node's own layout parameters, for O(1) access during layout
		// computation. Ownership stays with the parent that provided it in AddChild: the parent
		// allocates it, and this Node's parent is responsible for deleting it (see
		// Node::RemoveChild / Node::~Node), never this Node itself directly.
		LayoutParams*				_layoutParams = nullptr;

		Vector<Node*>				_children;

		PropertyChangedEvent		_propertyChangedEvent;
	};
}

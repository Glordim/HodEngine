#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline ui2
{
	class Node;

	/// @brief Layout parameters describing how a specific parent Node should arrange one of its
	/// children. Owned by the parent (via the child's cached pointer, see Node::_layoutParams),
	/// never by the child itself: the same reusable widget can be placed in different containers,
	/// each providing a different concrete LayoutParams instance.
	///
	/// Reflected (REFLECTED_CLASS_NO_PARENT rather than an ordinary abstract interface) so that
	/// GetReflectionDescriptorV() is virtual and concrete subtypes (see AnchoredLayoutParams) can
	/// be looked up and (de)serialized polymorphically through a base LayoutParams* — see
	/// LayoutParamsFactory.
	class HOD_UI2_API LayoutParams
	{
		REFLECTED_CLASS_NO_PARENT(LayoutParams)

	public:

		virtual						~LayoutParams() = default;

		virtual Vector2				ComputeSize(const Vector2& parentSize, const Vector2& desiredSize) const = 0;
		virtual Vector2				ComputePosition(const Vector2& parentSize, const Vector2& size) const = 0;

		/// @brief Whether the governed child's size depends on its parent's size (used to decide
		/// dirty-flag propagation when the parent is resized).
		virtual bool				DependsOnParentSize() const = 0;

	protected:

		void						MarkOwnerSizeAsDirty();
		void						MarkOwnerLocalMatrixAsDirty();

	private:

		friend class Node;
		Node*						_node = nullptr; // non-owning, set by Node::AddChild
	};
}

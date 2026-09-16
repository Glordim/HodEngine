#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline math
{
	struct Rect;
}

namespace hod::inline ui2
{
	class Node;
	class LayoutParams;

	/// @brief Arranges a Node's children. Attached to a Node via Node::SetLayout<T>() — a single,
	/// optional slot, never a collection: a node has at most one arrangement algorithm at a time,
	/// chosen independently of what the node itself paints/is (see BoxLayout). When no Layout is
	/// attached, Node falls back to its own built-in default (max-of-children "content" placement).
	///
	/// Reflected (REFLECTED_CLASS_NO_PARENT) so a concrete subtype (see BoxLayout) can be looked up
	/// and (de)serialized polymorphically through a base Layout* — see LayoutFactory.
	class HOD_UI2_API Layout
	{
		REFLECTED_CLASS_NO_PARENT(Layout)

	public:

		virtual						~Layout() = default;

		// Reflection-based equivalent of dynamic_cast<_Layout_*>(layout) — see LayoutParams::Cast.
		template<typename _Layout_>
		static _Layout_*			Cast(Layout* layout);

		// Returns the size this layout's content wants, given the space owner's parent is willing to
		// offer. Must measure every child of `owner` exactly once (via child->Measure(...)), even one
		// whose contribution isn't used until Arrange (e.g. a weighted Fill child) — Node::Measure()
		// applies owner's own LayoutParams override/clamp on top of whatever this returns, so this
		// only needs to report the content-derived size.
		virtual Vector2				Measure(Node& owner, const Vector2& availableSize) = 0;

		// Arranges owner's children within contentRect (owner's own content rect, already computed
		// and expressed in owner's local space by Node::BeginArrange()).
		virtual void				Arrange(Node& owner, const Rect& contentRect) = 0;

		// The LayoutParams type a child added via Node::AddChild(Node*) (no explicit LayoutParams)
		// should receive from an owner using this Layout.
		virtual LayoutParams*		CreateDefaultLayoutParams() const = 0;

	protected:

		void						MarkOwnerMeasureAsDirty();
		void						MarkOwnerArrangeAsDirty();

	private:

		friend class Node;
		Node*						_node = nullptr; // non-owning, set by Node::SetLayout
	};

	template<typename _Layout_>
	_Layout_* Layout::Cast(Layout* layout)
	{
		static_assert(std::is_base_of<Layout, _Layout_>::value, "_Layout_ must derive from Layout to use Layout::Cast()");

		if (layout != nullptr && layout->GetReflectionDescriptorV().IsCompatible(_Layout_::GetReflectionDescriptor()))
		{
			return static_cast<_Layout_*>(layout);
		}
		return nullptr;
	}
}

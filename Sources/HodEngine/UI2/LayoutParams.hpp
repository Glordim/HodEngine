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

		// Where the governed child's size comes from on one axis, when its parent's layout doesn't
		// impose it (stretched anchors, BoxLayoutParams::Rule::Fill, BoxLayoutParams' Stretch alignment...):
		// - Fixed: GetSize() on that axis
		// - Content: what the child measures (its children, its Layout, its text...), see Node::Measure
		enum class SizeMode : uint8_t { Fixed, Content };
		REFLECTED_ENUM(HOD_UI2_API, SizeMode);

	public:

		virtual						~LayoutParams() = default;

		// Reflection-based equivalent of dynamic_cast<_LayoutParams_*>(layoutParams): every
		// LayoutParams type already carries reflection metadata, so RTTI isn't needed to downcast
		// one safely. Null-safe and returns nullptr on a type mismatch, exactly like dynamic_cast.
		template<typename _LayoutParams_>
		static _LayoutParams_*		Cast(LayoutParams* layoutParams);
		template<typename _LayoutParams_>
		static const _LayoutParams_*	Cast(const LayoutParams* layoutParams);

		/// @brief Called on a LayoutParams replacing `previous`, of another type, on the same child (its parent's layout changed,
		/// see Node::SetLayout, or the child moved to another parent): the reflected properties of the types both derive from
		/// are already copied over. Carries what that copy can't express, e.g. a field each type names after its own axis.
		virtual void				CarryOverFrom(const LayoutParams& previous);

		SizeMode					GetWidthMode() const;
		void						SetWidthMode(SizeMode widthMode);

		SizeMode					GetHeightMode() const;
		void						SetHeightMode(SizeMode heightMode);

		// The child's size on its Fixed axes; unused on its Content ones. A delta on AnchoredLayoutParams' stretched axes.
		const Vector2&				GetSize() const;
		void						SetSize(const Vector2& size);

		// Bounds of the child's size, whatever it comes from (Fixed, Content or imposed by the parent).
		const Vector2&				GetMinSize() const;
		void						SetMinSize(const Vector2& minSize);

		const Vector2&				GetMaxSize() const;
		void						SetMaxSize(const Vector2& maxSize);

		/// @brief The child's own size: GetSize() on its Fixed axes, contentSize on its Content ones,
		/// clamped to [GetMinSize(), GetMaxSize()].
		Vector2						ResolveSize(const Vector2& contentSize) const;

		/// @brief Whether the governed child's size depends on its parent's size (used to decide
		/// dirty-flag propagation when the parent is arranged again): DependsOnParentWidth() || DependsOnParentHeight().
		bool						DependsOnParentSize() const;

		/// @brief Whether the governed child's size depends on its parent's on one axis (e.g. AnchoredLayoutParams' stretched anchors).
		/// Such a child doesn't count in its parent's content size on that axis (see Node::Measure): the parent can't be sized from it.
		virtual bool				DependsOnParentWidth() const;
		virtual bool				DependsOnParentHeight() const;

		/// @brief The size the child measures, given the space its parent offers. Base implementation:
		/// ResolveSize(contentSize). Overridden by a type whose parent imposes the size on some axes
		/// (e.g. AnchoredLayoutParams' stretched anchors); a Layout may still impose another one at
		/// Arrange (e.g. BoxLayout's Fill rule or Stretch alignment).
		virtual Vector2				ComputeSize(const Vector2& availableSize, const Vector2& contentSize) const;

		/// @brief Optional escape hatch for a LayoutParams type that positions its child outright
		/// within the parent's content rect (e.g. AnchoredLayoutParams), when no Layout is attached
		/// to the parent. Base implementation declines (returns false): the child is then stretched
		/// over the parent's content rect.
		virtual bool				TryComputePosition(const Rect& parentContentRect, const Vector2& size, Vector2& outPosition) const;

	protected:

		void						MarkOwnerMeasureAsDirty();
		void						MarkOwnerArrangeAsDirty();

		// The governed child, null until the LayoutParams is given to it (see Node::AddChild).
		Node*						GetNode() const;

	private:

		SizeMode					_widthMode = SizeMode::Fixed;
		SizeMode					_heightMode = SizeMode::Fixed;
		Vector2						_size = Vector2(100.0f, 100.0f);
		Vector2						_minSize = Vector2::Zero;
		Vector2						_maxSize = Vector2::Infinity;

	private:

		friend class Node;
		Node*						_node = nullptr; // non-owning, set by Node::AddChild
	};

	template<typename _LayoutParams_>
	_LayoutParams_* LayoutParams::Cast(LayoutParams* layoutParams)
	{
		static_assert(std::is_base_of<LayoutParams, _LayoutParams_>::value, "_LayoutParams_ must derive from LayoutParams to use LayoutParams::Cast()");

		if (layoutParams != nullptr && layoutParams->GetReflectionDescriptorV().IsCompatible(_LayoutParams_::GetReflectionDescriptor()))
		{
			return static_cast<_LayoutParams_*>(layoutParams);
		}
		return nullptr;
	}

	template<typename _LayoutParams_>
	const _LayoutParams_* LayoutParams::Cast(const LayoutParams* layoutParams)
	{
		return Cast<_LayoutParams_>(const_cast<LayoutParams*>(layoutParams));
	}
}

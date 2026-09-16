#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Margin.hpp>

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

		enum class HAlign : uint8_t { Left, Center, Right, Stretch };
		REFLECTED_ENUM(HOD_UI2_API, HAlign);

		enum class VAlign : uint8_t { Top, Center, Bottom, Stretch };
		REFLECTED_ENUM(HOD_UI2_API, VAlign);

	public:

		virtual						~LayoutParams() = default;

		// Reflection-based equivalent of dynamic_cast<_LayoutParams_*>(layoutParams): every
		// LayoutParams type already carries reflection metadata, so RTTI isn't needed to downcast
		// one safely. Null-safe and returns nullptr on a type mismatch, exactly like dynamic_cast.
		template<typename _LayoutParams_>
		static _LayoutParams_*		Cast(LayoutParams* layoutParams);

		const Margin&				GetMargin() const;
		void						SetMargin(const Margin& margin);

		HAlign						GetHAlign() const;
		void						SetHAlign(HAlign hAlign);

		VAlign						GetVAlign() const;
		void						SetVAlign(VAlign vAlign);

		const Vector2&				GetMinSize() const;
		void						SetMinSize(const Vector2& minSize);

		const Vector2&				GetMaxSize() const;
		void						SetMaxSize(const Vector2& maxSize);

		/// @brief Whether the governed child's size depends on its parent's size (used to decide
		/// dirty-flag propagation when the parent is arranged again).
		virtual bool				DependsOnParentSize() const;

		/// @brief Optional escape hatch for a LayoutParams type that determines its own child size
		/// outright (e.g. AnchoredLayoutParams), instead of the generic min/max-clamped desired size
		/// a container would otherwise use. Base implementation declines (returns false).
		virtual bool				TryComputeSize(const Vector2& availableSize, const Vector2& desiredSize, Vector2& outSize) const;

		/// @brief Optional escape hatch for a LayoutParams type that positions its child outright
		/// within the parent's content rect (e.g. AnchoredLayoutParams), instead of the generic
		/// margin/HAlign/VAlign placement a container would otherwise use. Base implementation
		/// declines (returns false).
		virtual bool				TryComputePosition(const Rect& parentContentRect, const Vector2& size, Vector2& outPosition) const;

	protected:

		void						MarkOwnerMeasureAsDirty();
		void						MarkOwnerArrangeAsDirty();

	private:

		Margin						_margin;
		HAlign						_hAlign = HAlign::Stretch;
		VAlign						_vAlign = VAlign::Stretch;
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
}

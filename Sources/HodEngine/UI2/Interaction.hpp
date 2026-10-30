#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <map>

namespace hod::inline ui2
{
	class Node;
	struct PointerEvent;

	/// @brief How a Node reacts to the pointer (see Button). Attached to a Node via Node::SetInteraction<T>(), a single,
	/// optional slot like Layout: a node has at most one Interaction, and combining two (e.g. a clickable card that can
	/// also be dragged) is done by nesting nodes, the event a node's Interaction doesn't handle bubbling up to its parent's.
	/// Chosen independently of what the node draws: any node can become interactive.
	///
	/// Reflected (REFLECTED_CLASS_NO_PARENT) so a concrete subtype can be (de)serialized polymorphically through a base
	/// Interaction*, see InteractionFactory.
	class HOD_UI2_API Interaction
	{
		REFLECTED_CLASS_NO_PARENT(Interaction)

	public:

		virtual						~Interaction() = default;

		// Reflection-based equivalent of dynamic_cast<_Interaction_*>(interaction) — see LayoutParams::Cast.
		template<typename _Interaction_>
		static _Interaction_*		Cast(Interaction* interaction);
		template<typename _Interaction_>
		static const _Interaction_*	Cast(const Interaction* interaction);

		Node*						GetNode() const;

		// Receives the pointer events the canvas dispatches to the owner node (see PointerEvent::Type for which ones bubble).
		// Returns true when handled: a bubbling event then stops here. Default: handles nothing.
		virtual bool				OnPointerEvent(const PointerEvent& event);

		// Advances what plays over time (e.g. Button's state transitions). Called by Canvas::Update, before the layout.
		virtual void				Update(float deltaTime);

		// Follows the nodes it references by local id (e.g. Button's overrides' targets) getting new ids (see Node::RenewLocalIds):
		// the ids found in oldToNew are replaced, the others kept if keepOthers, the references dropped otherwise. Default:
		// references none.
		virtual void				RemapTargets(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers);

	private:

		friend class Node;
		Node*						_node = nullptr; // non-owning, set by Node::SetInteraction
	};

	template<typename _Interaction_>
	_Interaction_* Interaction::Cast(Interaction* interaction)
	{
		static_assert(std::is_base_of<Interaction, _Interaction_>::value, "_Interaction_ must derive from Interaction to use Interaction::Cast()");

		if (interaction != nullptr && interaction->GetReflectionDescriptorV().IsCompatible(_Interaction_::GetReflectionDescriptor()))
		{
			return static_cast<_Interaction_*>(interaction);
		}
		return nullptr;
	}

	template<typename _Interaction_>
	const _Interaction_* Interaction::Cast(const Interaction* interaction)
	{
		return Cast<_Interaction_>(const_cast<Interaction*>(interaction));
	}
}

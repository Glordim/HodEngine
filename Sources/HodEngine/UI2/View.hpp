#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <map>

namespace hod::inline ui2
{
	class Node;
	class NodeRefBase;

	/// @brief The code side of a UI prefab: the interface gameplay talks to (SetScore, a click event...) instead of the prefab's
	/// tree, which only the view knows, through the nodes it references (see NodeRef, ViewRef), assigned in the editor. Moving
	/// or renaming nodes in the prefab doesn't break it: the references follow the nodes' local ids.
	///
	/// Attached to a Node (the prefab's root, normally) via Node::SetView, a single, optional slot like Layout and Interaction.
	/// A game module declares its views (REFLECTED_CLASS(MyView, ui2::View)), their references as reflected properties, and
	/// registers them in ViewFactory. A nested prefab instance has its prefab's view: the containing view reaches it through a
	/// ViewRef, each view only knowing its own prefab.
	///
	/// A view runs only once bound (see Bind), which whoever runs the prefab does (UIComponent, in game), never the prefab's
	/// editor: OnBind is where it connects to its nodes' events (Button::GetClickEvent...) and sets up what it shows.
	class HOD_UI2_API View
	{
		REFLECTED_CLASS_NO_PARENT(View)

	public:

		virtual						~View() = default;

		// Reflection-based equivalent of dynamic_cast<_View_*>(view) — see LayoutParams::Cast.
		template<typename _View_>
		static _View_*				Cast(View* view);
		template<typename _View_>
		static const _View_*		Cast(const View* view);

		Node*						GetNode() const;

		// Resolves the references (in the scope of the view's node, see Node::GetScopeRoot) then calls OnBind. The references
		// to a node missing, or not of the expected type, stay null. Does nothing if already bound.
		void						Bind();
		// Calls OnUnbind then drops the resolved references. Does nothing if not bound. Done by the node before it is deleted.
		void						Unbind();
		bool						IsBound() const;

		// The references the view holds (its reflected NodeRef / ViewRef properties, its parents' included)
		void						CollectReferences(Vector<NodeRefBase*>& outReferences);
		// Follows the nodes it references getting new ids (see Node::RenewLocalIds, Interaction::RemapTargets)
		void						RemapReferences(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers);

	protected:

		// The references are resolved, and the views of the nodes below already bound: a view can use its nested views.
		virtual void				OnBind();
		// Before the nodes go: disconnect from what isn't the prefab's.
		virtual void				OnUnbind();

	private:

		friend class Node;
		Node*						_node = nullptr; // non-owning, set by Node::SetView
		bool						_bound = false;
	};

	template<typename _View_>
	_View_* View::Cast(View* view)
	{
		static_assert(std::is_base_of<View, _View_>::value, "_View_ must derive from View to use View::Cast()");

		if (view != nullptr && view->GetReflectionDescriptorV().IsCompatible(_View_::GetReflectionDescriptor()))
		{
			return static_cast<_View_*>(view);
		}
		return nullptr;
	}

	template<typename _View_>
	const _View_* View::Cast(const View* view)
	{
		return Cast<_View_>(const_cast<View*>(view));
	}
}

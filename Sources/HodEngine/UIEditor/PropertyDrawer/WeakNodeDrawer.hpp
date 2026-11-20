#pragma once
#include "HodEngine/UIEditor/Export.hpp"
#include "HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp"

namespace hod::inline ui
{
	class Node;
	class WeakNodeBase;
}

namespace hod::inline editor
{
	class UIPrefabEditorTab;

	/// @brief A ui::View's reference to a node (ui::WeakNode, ui::WeakView): the node referenced, picked among the nodes of the
	/// view's scope that are of the expected type, or dropped from the hierarchy. Shown as missing when the node isn't there
	/// anymore, or isn't of that type anymore. Only drawn within a Scope, which says where the nodes are.
	class HOD_UI_EDITOR_API WeakNodeDrawer : public PropertyDrawer<WeakNodeBase>
	{
	public:
		/// @brief While alive, the references drawn are the ones of a view of tab, resolved in scopeRoot (see ui::Node::GetScopeRoot)
		class HOD_UI_EDITOR_API Scope
		{
		public:
							Scope(UIPrefabEditorTab& tab, ui::Node& scopeRoot);
							Scope(const Scope&) = delete;
							Scope(Scope&&) = delete;
							~Scope();

			Scope&			operator=(const Scope&) = delete;
			Scope&			operator=(Scope&&) = delete;
		};

	protected:
		bool OnDraw(const PropertyValues<WeakNodeBase>& values) override;
	};
}

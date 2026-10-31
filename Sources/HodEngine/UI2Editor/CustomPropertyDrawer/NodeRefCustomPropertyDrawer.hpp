#pragma once
#include "HodEngine/UI2Editor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline ui2
{
	class Node;
}

namespace hod::inline editor
{
	class UIPrefabEditorTab;

	/// @brief A ui2::View's reference to a node (ui2::NodeRef, ui2::ViewRef): the node referenced, picked among the nodes of the
	/// view's scope that are of the expected type, or dropped from the hierarchy. Shown as missing when the node isn't there
	/// anymore, or isn't of that type anymore. Only drawn within a Scope, which says where the nodes are.
	class HOD_UI2_EDITOR_API NodeRefCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:

		/// @brief While alive, the references drawn are the ones of a view of tab, resolved in scopeRoot (see ui2::Node::GetScopeRoot)
		class HOD_UI2_EDITOR_API Scope
		{
		public:
							Scope(UIPrefabEditorTab& tab, ui2::Node& scopeRoot);
							Scope(const Scope&) = delete;
							Scope(Scope&&) = delete;
							~Scope();

			Scope&			operator=(const Scope&) = delete;
			Scope&			operator=(Scope&&) = delete;
		};

	public:

							NodeRefCustomPropertyDrawer() = default;
							NodeRefCustomPropertyDrawer(const NodeRefCustomPropertyDrawer&) = delete;
							NodeRefCustomPropertyDrawer(NodeRefCustomPropertyDrawer&&) = delete;
							~NodeRefCustomPropertyDrawer() override = default;

		NodeRefCustomPropertyDrawer&	operator=(const NodeRefCustomPropertyDrawer&) = delete;
		NodeRefCustomPropertyDrawer&	operator=(NodeRefCustomPropertyDrawer&&) = delete;

	public:

		bool				Draw(EditorReflectedProperty& editorReflectedProperty, bool onlyValue = false) override;
	};
}

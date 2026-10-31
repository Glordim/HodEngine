#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/Node.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

#include <cstdint>
#include <map>

namespace hod::inline ui2
{
	/// @brief A View's reference to a node of its prefab (see View), assigned in the editor: serialized as the node's local id
	/// (see Node::GetLocalId), so renaming or moving the node keeps it. Resolved when the view is bound (see View::Bind), null
	/// before that, and when the node isn't there anymore or isn't of the type the reference expects.
	///
	/// Declared as NodeRef<_Node_> (a node of that type) or ViewRef<_View_> (a node carrying a view of that type, e.g. a nested
	/// prefab instance's root), as a direct member of the View, reflected like any property.
	class HOD_UI2_API NodeRefBase
	{
		REFLECTED_CLASS_NO_VIRTUAL(NodeRefBase)

	public:

							NodeRefBase() = default;
							NodeRefBase(const ReflectionDescriptor* nodeDescriptor, const ReflectionDescriptor* viewDescriptor);

		bool				operator==(const NodeRefBase& other) const;

		// The node's id in the view's scope (see Node::FindByLocalId), 0 for none. Setting it drops the resolved node.
		uint64_t			GetLocalId() const;
		void				SetLocalId(uint64_t localId);

		// The type the node must have (null: any), and the type of view it must carry (null: none needed)
		const ReflectionDescriptor*	GetNodeDescriptor() const;
		const ReflectionDescriptor*	GetViewDescriptor() const;
		// Whether node is of the types this reference expects
		bool				Accepts(const Node& node) const;

		// The node referenced, once resolved (see Resolve). Null otherwise.
		Node*				GetNode() const;

		// Finds the node in scopeRoot's scope. A node missing, or not of the expected types, gives null (with a warning).
		// @return whether a node was found, false also for no reference
		bool				Resolve(Node& scopeRoot);
		void				Reset();

		// Follows the node getting a new id (see Node::RenewLocalIds): an id found in oldToNew is replaced, the others kept if
		// keepOthers, dropped otherwise.
		void				Remap(const std::map<uint64_t, uint64_t>& oldToNew, bool keepOthers);

	private:

		uint64_t					_localId = 0;
		const ReflectionDescriptor*	_nodeDescriptor = nullptr;
		const ReflectionDescriptor*	_viewDescriptor = nullptr;
		Node*						_node = nullptr;
	};

	/// @brief A node of type _Node_ (see NodeRefBase)
	template<typename _Node_>
	class NodeRef : public NodeRefBase
	{
	public:

		NodeRef()
		: NodeRefBase(&_Node_::GetReflectionDescriptor(), nullptr)
		{
			static_assert(std::is_base_of<Node, _Node_>::value, "_Node_ must derive from Node to use NodeRef");
		}

		_Node_*		Get() const { return static_cast<_Node_*>(GetNode()); }
		_Node_*		operator->() const { return Get(); }
		explicit	operator bool() const { return GetNode() != nullptr; }
	};

	/// @brief A node carrying a view of type _View_ (see NodeRefBase), giving that view: typically a nested prefab instance, which
	/// the containing prefab's view drives through its own view rather than through its nodes.
	template<typename _View_>
	class ViewRef : public NodeRefBase
	{
	public:

		// Checked here rather than in the class: a view may reference views of its own type, incomplete there
		ViewRef()
		: NodeRefBase(nullptr, &_View_::GetReflectionDescriptor())
		{
			static_assert(std::is_base_of<View, _View_>::value, "_View_ must derive from View to use ViewRef");
		}

		_View_*		Get() const { return (GetNode() != nullptr) ? GetNode()->template GetView<_View_>() : nullptr; }
		_View_*		operator->() const { return Get(); }
		explicit	operator bool() const { return Get() != nullptr; }
	};
}

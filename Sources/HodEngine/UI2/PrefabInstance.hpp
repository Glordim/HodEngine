#pragma once
#include "HodEngine/UI2/Export.hpp"
#include "HodEngine/UI2/PropertyOverride.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/UID.hpp>
#include <HodEngine/Core/Vector.hpp>

namespace hod::inline ui2
{
	class Node;

	/// @brief What makes a Node the root of an instance of another UI prefab (nested prefab): the subtree is built from that
	/// prefab's root (the instance's source), then given the instance's overrides (see PropertyOverride), by the local ids of the
	/// prefab's nodes. Only the instance's placement (its LayoutParams, given by the parent it is in) and its overrides belong to
	/// the containing prefab: the structure follows the prefab's.
	///
	/// The nodes of the subtree keep the local ids they have in their prefab: an instance is a scope of local ids of its own
	/// (Node::FindByLocalId doesn't cross into it), its root being seen from the containing prefab by an id of the containing
	/// prefab's (Node::GetLocalId), and from inside the instance by its id in the prefab (GetRootLocalId).
	///
	/// A prefab that can't be loaded (deleted, or containing itself, see ScopedLoading) gives a placeholder: an empty Node keeping
	/// the instance's data, so that saving the containing prefab doesn't lose them.
	class HOD_UI2_API PrefabInstance
	{
	public:
		/// @brief Where the prefabs' documents come from (the "Nodes" document, as UIPrefabResource holds it). By default the cooked
		/// resources, through the ResourceManager: the editor reads its assets instead, so an instance follows its prefab as last saved.
		using SourceProvider = bool (*)(const UID& prefab, Document& outDocument);

		/// @brief While alive, prefab is being loaded (by its editor, as a resource or as an instance): an instance of it met
		/// meanwhile is a placeholder, as a prefab can't contain itself. Per thread.
		class HOD_UI2_API ScopedLoading
		{
		public:
							ScopedLoading(const UID& prefab);
							ScopedLoading(const ScopedLoading&) = delete;
							ScopedLoading(ScopedLoading&&) = delete;
							~ScopedLoading();

			ScopedLoading&	operator=(const ScopedLoading&) = delete;
			ScopedLoading&	operator=(ScopedLoading&&) = delete;
		};

	public:
		// nullptr goes back to the default one
		static void			SetSourceProvider(SourceProvider provider);
		static bool			LoadSource(const UID& prefab, Document& outDocument);
		static bool			IsLoading(const UID& prefab);

		// The prefabs instantiated in entry (a node as Node::SerializeAsChildInDocument writes it) and its subtree, not the ones
		// instantiated in these prefabs
		static void			CollectPrefabs(const DocumentNode& entry, Vector<UID>& outPrefabs);
		// Whether prefab is target or contains an instance of it, at any depth.
		static bool			Contains(const UID& prefab, const UID& target);

		// A prefab's source changed (emitted by the editor once a UI prefab is saved): what was built from it, or from a prefab
		// containing it (see Contains), can be built again to follow it.
		using SourceChangedEvent = Event<const UID&>;
		static SourceChangedEvent&	GetSourceChangedEvent();

	public:
							PrefabInstance(const UID& prefab);
							PrefabInstance(const PrefabInstance&) = delete;
							PrefabInstance(PrefabInstance&&) = delete;
							~PrefabInstance();

		PrefabInstance&		operator=(const PrefabInstance&) = delete;
		PrefabInstance&		operator=(PrefabInstance&&) = delete;

	public:
		const UID&			GetPrefab() const;
		// The prefab couldn't be loaded: the instance is a placeholder
		bool				IsMissing() const;
		// The instance root's local id in its prefab
		uint64_t			GetRootLocalId() const;

		// The prefab's node of that local id as authored, without the instance's overrides (meant for the editor: to show and revert
		// the overrides). Built on the first call, then kept. Null when missing, or if there is no such node.
		Node*				FindSourceNode(uint64_t localId);

		// What differs between instanceRoot (whose instance this is) and the source: the overridable properties (with the
		// Interactions') of the nodes of the instance's own scope, the nested instances' roots included but not their content,
		// except the root's LayoutParams, which belong to the containing prefab. The overrides loaded when missing.
		void				CollectOverrides(Node& instanceRoot, Vector<PropertyOverride>& outOverrides);

	private:
		friend class Node;

		UID							_prefab;
		uint64_t					_rootLocalId = 0;
		Document					_source; // The prefab root's entry (see Node::SerializeAsChildInDocument), empty when missing
		bool						_missing = true;
		Vector<PropertyOverride>	_missingOverrides; // As loaded, kept while missing
		Node*						_sourceContainer = nullptr; // Parent of the source, built by FindSourceNode
	};
}

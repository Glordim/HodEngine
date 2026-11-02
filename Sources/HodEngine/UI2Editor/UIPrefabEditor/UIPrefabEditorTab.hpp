#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/UI2/Selectable.hpp>
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

#include <HodEngine/Core/Event.hpp>
#include <HodEngine/Core/UID.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
	class DocumentNode;
}

namespace hod::inline ui2
{
	class Node;
	struct AnimationClip;
}

namespace hod::inline editor
{
	/// @brief Editor tab for a UIPrefabResource asset: a tree of ui2::Node edited directly (no
	/// Entity/World involved), the UI2 counterpart of PrefabEditorTab.
	class HOD_UI2_EDITOR_API UIPrefabEditorTab : public EditorTab
	{
	public:

						UIPrefabEditorTab(std::shared_ptr<Asset> asset);
						~UIPrefabEditorTab() override;

		ui2::Canvas&	GetCanvas();

		// The prefab's actual root: the single child of the canvas' own root node, which is only the
		// hidden container (design resolution sized) playing the parent that arranges it. Never null,
		// see EnsurePrefabRoot().
		ui2::Node*		GetPrefabRoot() const;

		// Selection: any number of nodes, the last one selected being the active one (the one the inspector shows, F2 renames,
		// F frames...).
		// The active node, nullptr if none
		ui2::Node*					GetSelectedNode() const;
		// In the order they were selected, the active one last
		const Vector<ui2::Node*>&	GetSelectedNodes() const;
		bool						IsSelected(const ui2::Node* node) const;
		// node only, nothing if nullptr
		void						SetSelectedNode(ui2::Node* node);
		// nodes only, the last one active
		void						SetSelectedNodes(const Vector<ui2::Node*>& nodes);
		// Adds node, as the active one, or removes it
		void						SetNodeSelected(ui2::Node* node, bool selected);
		// The selected nodes but those inside another selected one, in the hierarchy's order: the subtrees given to what acts on
		// a node along with its descendants (copy, delete, move...)
		Vector<ui2::Node*>			GetSelectedSubtrees() const;

		// The canvas itself (its settings, e.g. the scaler) as the selection, shown as the hierarchy's top
		// item. Exclusive with the selected nodes: selecting either deselects the other.
		bool			IsCanvasSelected() const;
		void			SelectCanvas();

		// Asks the viewport to center and zoom on the selection (F, from the hierarchy). Consumed by the
		// viewport on its next frame.
		void			RequestFrameSelection();
		bool			ConsumeFrameSelectionRequest();

		// What can be done to a node, given the nested prefab instances it is in (see ui2::PrefabInstance)
		enum class NodeRole : uint8_t
		{
			Authored,        // A node of this prefab: anything goes
			InstanceRoot,    // The root of a nested prefab's instance: placed, moved and removed as any node, but its content follows its prefab
			InstanceContent, // A node of an instance (not its root): only its properties can be changed, recorded as the instance's overrides
			ReadOnly,        // A node of an instance held by an instance: edited in its own prefab only
		};
		static NodeRole	GetNodeRole(const ui2::Node* node);
		// Children added, type converted, Layout or Interaction added or removed: Authored nodes only
		static bool		CanEditStructure(const ui2::Node* node);
		// Deleted, moved, cut or duplicated: Authored nodes and instance roots, except the prefab root
		bool			CanRemove(const ui2::Node* node) const;
		// Properties edited: every node but the ReadOnly ones and the missing prefabs' placeholders
		static bool		CanEditProperties(const ui2::Node* node);
		// The root of the prefab its tab edits (the single child of the canvas' hidden container, see GetPrefabRoot)
		static bool		IsPrefabRoot(const ui2::Node* node);
		// Renamed: what can have its properties edited but the prefab root, always named after its asset (rename the asset to
		// rename it, see NamePrefabRootAfterAsset)
		static bool		CanRename(const ui2::Node* node);
		// Gives the root of a UI prefab's document (as its tab saves it, see OnSave) the name of its asset: what its instances
		// are named after, without an override each
		// @return true if the name changed
		static bool		NamePrefabRootAfterAsset(Document& prefabDocument, std::string_view assetName);
		// For an instance's root or content: the node as its prefab authors it (see ui2::PrefabInstance::FindSourceNode), what
		// its properties are compared to (overrides) and reverted to. Null for the other nodes.
		static ui2::Node*	FindSourceNode(const ui2::Node* node);

		// Replaces node by a new node of the type nodeDescriptor describes (see ui2::Node::ReplaceWith),
		// keeps it selected if it was (in its place in the selection), and deletes node: callers must not be drawing node (or iterating
		// its children) anymore, i.e. defer the call to after their drawing pass.
		// @return the new node, nullptr if node's structure can't be edited (see CanEditStructure)
		ui2::Node*		ConvertNode(ui2::Node* node, const ReflectionDescriptor& nodeDescriptor);

		// Removes nodes (and their subtrees) from the prefab, deselecting them and their descendants. The nodes that can't be
		// removed (see CanRemove) are skipped. Same contract as ConvertNode: defer the call to after the drawing pass.
		// @return false if none was removed
		bool			DeleteNodes(const Vector<ui2::Node*>& nodes);

		// Nested prefabs: adds an instance of prefab (a UI prefab asset) as the last child of parent, selected. Refused (with an
		// error) if prefab is this one or contains it, as it would contain itself. Same contract as ConvertNode.
		// @return the instance's root, nullptr if refused
		ui2::Node*		InstantiatePrefab(const UID& prefab, ui2::Node* parent);
		// Turns an instance into plain nodes of this prefab (see ui2::Node::UnpackPrefabInstance), to edit them freely.
		bool			UnpackPrefabInstance(ui2::Node* instanceRoot);
		// Makes a new UI prefab asset in folder, named after node, of node and its subtree as authored (with new local ids: the
		// references its Interactions hold to nodes outside of it are dropped), then replaces node by an instance of it, keeping
		// its placement, its local id and the selection. Only for Authored nodes but the prefab root (an instance is unpacked
		// first), not while recording or playing. Same contract as ConvertNode.
		// @return the new asset, nullptr if refused (with an error) or failed
		std::shared_ptr<Asset>	CreatePrefabFromNode(ui2::Node* node, const Path& folder);
		// Writes a new UI prefab asset at assetPath as its tab makes an empty one (see CreateDefaultPrefabRoot), with its thumbnail
		// @return false if it can't be written
		static bool				CreateEmptyPrefab(const Path& assetPath);
		// The open UI prefab tab editing node, nullptr if none
		static UIPrefabEditorTab*	FindTabOf(const ui2::Node* node);
		// The UI prefab asset prefab written outside of its tab (renamed...): the open tabs rebuild their instances of it, as when
		// it is saved (see RefreshInstances)
		static void					NotifyPrefabWritten(const UID& prefab);
		// Bumped whenever instances are rebuilt from their saved prefab (see RefreshInstances): the nodes they had are deleted, a
		// window keeping node pointers across frames drops them on a change.
		uint32_t		GetRebuildCount() const;

		// Clipboard: nodes and their subtrees (one inside another of them comes with it) are copied as text (a JSON document,
		// see ui2::Node::SerializeAsChildInDocument) to the system clipboard, so they can be pasted in another UI prefab too.
		// Copied as authored: while recording, without the recorded state's look. Cancels a pending cut.
		bool			CopyNodes(const Vector<ui2::Node*>& nodes);
		// As in a file explorer: nodes are copied (see CopyNodes) and marked as cut, but stay in place until pasted in this
		// prefab, which moves them (see PasteNodes). Cancelled by CancelCut (Escape), another copy, or the clipboard changing.
		// @return false if one of them can't be moved (see CanRemove): nothing is cut then
		bool			CutNodes(const Vector<ui2::Node*>& nodes);
		void			CancelCut();
		// Whether node, or one of its ancestors, is marked as cut (see CutNodes).
		bool			IsCut(const ui2::Node* node) const;
		bool			HasCutNodes() const;
		// Whether the clipboard holds copied nodes.
		bool			CanPasteNode() const;
		// Pastes the clipboard's nodes, selected, in their copied order from index among parent's children (the last place if out
		// of range), with new local ids (see ui2::Node::RenewLocalIds). The references their Interactions hold to nodes outside of
		// the copied subtrees are kept when pasted in the prefab they were copied from, dropped otherwise. Nodes cut in this
		// prefab (see CutNodes) are moved there instead, keeping their ids, unless parent is one of them or inside one (nothing
		// is done then). Same contract as ConvertNode: defer the call to after the drawing pass.
		// @return the pasted (or moved) nodes, empty if nothing was pasted
		Vector<ui2::Node*>	PasteNodes(ui2::Node* parent, uint32_t index = UINT32_MAX);
		// Pastes right after the active node among its siblings (to chain pastes), or as the last children of the prefab root
		// when it is the selection (or there is none).
		Vector<ui2::Node*>	PasteNodesNextToSelection();
		// Pastes as the last children of the active node, or of the prefab root when there is no selection.
		Vector<ui2::Node*>	PasteNodesInSelection();
		// Places a copy of each node (but those inside another one of them) right after it among its siblings, the copies
		// selected, without going through the clipboard. The ones that can't be moved (see CanRemove, e.g. the prefab root which
		// can't have siblings) are skipped. Same contract as ConvertNode.
		// @return the copies
		Vector<ui2::Node*>	DuplicateNodes(const Vector<ui2::Node*>& nodes);
		// Whether ReparentNodes would move something: they can all be moved (see CanRemove), parent can have children (see
		// CanEditStructure), it is none of them nor inside one, and one of them at least isn't already its child.
		bool			CanReparentNodes(const Vector<ui2::Node*>& nodes, const ui2::Node* parent) const;
		// Moves nodes (but those inside another one of them), in the hierarchy's order, as the last children of parent, selected.
		// Refused if CanReparentNodes is false. Same contract as ConvertNode.
		bool			ReparentNodes(const Vector<ui2::Node*>& nodes, ui2::Node* parent);
		// Ctrl+C / Ctrl+X / Ctrl+V (next to the selection) / Ctrl+Shift+V (in the selection) / Ctrl+D on the selection, Escape
		// cancelling a cut, for the focused window calling it. Same contract as ConvertNode.
		void			HandleClipboardShortcuts();
		// Gives a local id to the nodes that have none yet (they are otherwise given on save): for something to reference
		// them right away, e.g. a property override's target.
		void			AssignLocalIds();

		// Record mode: the prefab shows a look of a Selectable (a Button's state, a Toggle's On...: its overrides applied, see
		// ui2::Selectable::GetLookOverrides), and editing the nodes' overridable properties
		// (theirs, their LayoutParams' and Layout's: see ui2::PropertyValue) records the look's overrides instead of changing the
		// prefab: the state's overrides are kept equal to what differs from the prefab as authored, until StopRecording() puts
		// that back. Other edits (nodes added, removed, moved, layout type changed...) still change the prefab itself. Saving while recording saves the prefab as
		// authored, plus the overrides recorded so far, and keeps recording.
		bool			IsRecording() const;
		bool			IsRecording(const ui2::Node* selectableNode, uint32_t look) const;
		const char*		GetRecordedStateName() const;
		void			StartRecording(ui2::Node* selectableNode, uint32_t look);
		void			StopRecording();

		// While recording, the override of that property is removed by putting the property back to its authored value.
		void			RevertRecordedProperty(uint64_t target, const String& property);

		// Play mode (the menu bar's Play / Pause / Step, as in the scene and prefab tabs): the viewport's pointer drives the
		// prefab's Interactions, to check hit-tests and state changes. Played on a copy of the canvas, made by Play() and dropped
		// by Stop(), so nothing the Interactions change ends up in the prefab. Editing is suspended meanwhile. Paused, neither
		// the pointer nor the time go on, but one frame at a time (PlayNextFrame).
		void			Play();
		void			Stop();
		void			Pause();
		void			Resume();
		void			PlayNextFrame(); // pauses if playing

		bool			IsPlaying() const;
		bool			IsPaused() const;

		// Whether the viewport plays a frame now: playing and not paused, or a step was asked (consumed)
		bool			ConsumeFrame();

		// The canvas the viewport shows: the play mode's copy while playing, the edited canvas otherwise.
		ui2::Canvas&	GetDisplayedCanvas();
		ui2::Node*		GetDisplayedPrefabRoot() const;

		// The actions the play mode's canvas emitted (see ui2::Canvas::GetActionEvent), most recent last. Cleared on entering it.
		struct InteractionLogEntry
		{
			String		_action;
			String		_nodeName;
			double		_time = 0.0; // ImGui::GetTime() when emitted
		};
		const Vector<InteractionLogEntry>& GetInteractionLog() const;

		// Animation preview (the Animation window): the prefab shows one of its clips (see ui2::Node::GetAnimations, on the prefab
		// root) at the timeline's time, over the prefab as authored, as state recording shows a state (exclusive with it). Edits
		// change the prefab as authored, or, with the animation recording on, add keys at that time to the properties edited.
		// Saved and undone as authored, the preview going on.
		bool			IsPreviewingAnimation() const;
		// The previewed clip, null if none (or gone)
		ui2::AnimationClip*	GetPreviewedAnimation() const;
		void			StartAnimationPreview(const String& clipName);
		void			StopAnimationPreview();
		float			GetAnimationTime() const;
		void			SetAnimationTime(float time);
		bool			IsRecordingAnimation() const;
		void			SetRecordingAnimation(bool recording);
		// The timeline playing, looping when the clip isn't Once
		bool			IsPlayingAnimation() const;
		void			SetPlayingAnimation(bool playing);
		// The clip's keys, tracks or duration edited (not through the recording): shown again at the current time. Call it
		// before MarkAsDirty.
		void			RefreshAnimationPreview();
		// While previewing: the value the prefab as authored gives that property (the clip's values left out). False for a node
		// added since the preview started, or not of this prefab.
		bool			GetAuthoredValue(uint64_t target, const String& property, ui2::PropertyValue& outValue) const;

		// The prefab root's clips, by name: a new one named uniquely (previewed), renamed (refused if the name is taken or
		// empty), deleted (the preview stopped if it was the previewed one).
		ui2::AnimationClip*	AddAnimation();
		bool			RenameAnimation(const String& clipName, const String& newName);
		bool			DeleteAnimation(const String& clipName);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

		// Undo: the canvas as OnSave writes it (as authored, plus the recorded state's overrides). Restoring it rebuilds every node
		// (see GetRebuildCount), the selection kept by the nodes' ids, recording going on if its Selectable is still there.
		bool	CaptureUndoState(Document& document) override;
		bool	RestoreUndoState(const Document& document) override;
		// Not while playing: editing is suspended
		bool	IsUndoAvailable() const override;

	private:

		void	LoadNodes();

		// Guarantees the canvas' root node holds exactly one child (the prefab root): creates it (an Image) for an
		// empty prefab, or, for one saved when the canvas' root node itself was the prefab root (possibly
		// with several children), wraps those children in a stretched prefab root so they keep their
		// placement.
		void	EnsurePrefabRoot();
		// Names the prefab root after the asset (see CanRename): on load, undo/redo, and each frame for an asset renamed meanwhile
		void	SyncPrefabRootName();
		// The root of a new prefab: an Image rather than a plain Node, drawn (its color filling it) for its preview and thumbnail
		// not to be empty, named name
		static ui2::Node*	CreateDefaultPrefabRoot(std::string_view name);

		void	GenerateThumbnail();
		// The thumbnail of the prefab canvas holds (see GenerateThumbnail), encoded (see Thumbnail)
		static bool	RenderThumbnail(ui2::Canvas& canvas, Vector<uint8_t>& outEncoded);

		// Clipboard (see CopyNode)
		// Each node of nodes as an unnamed child of documentNode
		bool		SerializeNodesAsAuthored(const Vector<ui2::Node*>& nodes, DocumentNode& documentNode);
		// Not selected: up to the callers
		ui2::Node*	InsertNodeCopy(ui2::Node* parent, uint32_t index, const DocumentNode& documentNode, bool keepOutsideTargets);
		void		MoveNode(ui2::Node* node, ui2::Node* parent, uint32_t index);
		// The nodes but those inside another one of them, in the hierarchy's order (the ones out of the prefab dropped)
		Vector<ui2::Node*>	GetSubtrees(const Vector<ui2::Node*>& nodes) const;
		// Deselects node and its descendants
		// @return whether one of them was selected
		bool		DeselectSubtree(const ui2::Node* node);
		// Whether adding the node entry describes would make this prefab contain itself (through the instances it holds)
		bool		WouldContainItself(const DocumentNode& entry) const;

		// Nested prefabs: a UI prefab saved (in any tab), the instances of it (also inside other instances) are rebuilt from
		// what it is now, keeping their overrides. Their nodes are replaced.
		using PrefabSavedEvent = Event<const UID&>;
		static PrefabSavedEvent&	GetPrefabSavedEvent();
		void						RefreshInstances(const UID& savedPrefab);
		void						RebuildInstance(ui2::Node* instanceRoot);

		// Undo (see RestoreUndoState): node's local ids from the prefab root down, one per nested prefab instance it is in (the
		// instance root's in its containing prefab, then the node's in the instance's prefab), found back by FindByScopedLocalIds
		// in another tree of the same prefab. Empty if one of them has no id.
		static Vector<uint64_t>	GetScopedLocalIds(const ui2::Node* node);
		ui2::Node*				FindByScopedLocalIds(const Vector<uint64_t>& scopedLocalIds) const;

		// Record mode (see StartRecording)
		// Takes the overridable properties of every node as they are, as the authored values recording compares them to
		void			CaptureRecordedBase();
		ui2::Selectable*	FindRecordedSelectable() const;
		void			UpdateRecording();
		void			RestoreRecordedBase();
		// Gives the nodes the recorded state's overrides, or the previewed clip's values (see ApplyAnimationSample)
		void			ApplyRecordedOverrides();
		// State recording or animation preview: the nodes show something else than the prefab as authored (_recordedBase)
		bool			HasRecordedBase() const;

		// Animation preview (see StartAnimationPreview)
		// The prefab as authored, then the clip's values at the current time
		void			ApplyAnimationSample();
		// The properties edited since the last ApplyAnimationSample: keyed at the current time when recording the animation,
		// taken as authored otherwise
		void			UpdateAnimationPreview();
		void			ClearAnimationPreview();

	private:

		ui2::Canvas	_canvas;
		Vector<ui2::Node*>	_selectedNodes; // the active one last
		bool		_canvasSelected = false;
		bool		_frameSelectionRequested = false;

		// Counter the nodes' local ids are taken from (see ui2::Node::GetLocalId). Persisted next to the
		// nodes in the asset's document: it belongs to the authored prefab, not to the canvas.
		uint64_t	_nextLocalId = 1;

		// Record mode: the recorded Selectable's node (local id, 0 when not recording), its look, and the overridable properties
		// of every node as authored when recording started
		struct RecordedBaseValue
		{
			uint64_t			_target = 0;
			String				_property;
			ui2::PropertyValue	_value;
		};
		uint64_t					_recordedNode = 0;
		uint32_t					_recordedLook = 0;
		Vector<RecordedBaseValue>	_recordedBase;

		// Animation preview: the clip's name (empty when none), the timeline's time, and the values of _recordedBase's properties
		// as the last ApplyAnimationSample left them (same order), to tell the edits made since
		String						_previewedAnimation;
		float						_animationTime = 0.0f;
		bool						_recordingAnimation = false;
		bool						_playingAnimation = false;
		bool						_animationPlayingBackwards = false; // Ping Pong's way back
		Vector<ui2::PropertyValue>	_animationDisplayedValues;

		ui2::Canvas*					_interactionCanvas = nullptr; // Play mode's copy, null when not playing
		bool							_paused = false;
		bool							_nextFrameRequested = false;
		ui2::Canvas::ActionEvent::Slot	_onInteractionAction;
		Vector<InteractionLogEntry>		_interactionLog;

		// Cut (see CutNodes): the cut nodes' local ids (empty when none), and the clipboard's text the cut put there, which must
		// still be the clipboard's for a paste to move the nodes
		Vector<uint64_t>	_cutNodes;
		String		_cutClipboardText;

		// Nested prefabs (see RefreshInstances): the save is told once written, i.e. on the next frame
		PrefabSavedEvent::Slot	_onPrefabSaved;
		bool					_savedNotificationPending = false;
		uint32_t				_rebuildCount = 0;
	};
}

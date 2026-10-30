#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/UI2/Button.hpp>
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
	class DocumentNode;
}

namespace hod::inline ui2
{
	class Node;
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

		ui2::Node*		GetSelectedNode() const;
		void			SetSelectedNode(ui2::Node* node);

		// The canvas itself (its settings, e.g. the scaler) as the selection, shown as the hierarchy's top
		// item. Exclusive with a selected node: selecting either one deselects the other.
		bool			IsCanvasSelected() const;
		void			SelectCanvas();

		// Asks the viewport to center and zoom on the selection (F, from the hierarchy). Consumed by the
		// viewport on its next frame.
		void			RequestFrameSelection();
		bool			ConsumeFrameSelectionRequest();

		// Replaces node by a new node of the type nodeDescriptor describes (see ui2::Node::ReplaceWith),
		// keeps it selected if it was, and deletes node: callers must not be drawing node (or iterating
		// its children) anymore, i.e. defer the call to after their drawing pass.
		// @return the new node
		ui2::Node*		ConvertNode(ui2::Node* node, const ReflectionDescriptor& nodeDescriptor);

		// Removes node (and its subtree) from the prefab, deselecting it if it, or one of its descendants, was
		// selected. Same contract as ConvertNode: defer the call to after the drawing pass.
		// @return false for the prefab root, which can't be deleted
		bool			DeleteNode(ui2::Node* node);

		// Clipboard: a node and its subtree are copied as text (a JSON document, see ui2::Node::SerializeAsChildInDocument) to the
		// system clipboard, so they can be pasted in another UI prefab too. Copied as authored: while recording, without the
		// recorded state's look. Cancels a pending cut.
		bool			CopyNode(ui2::Node* node);
		// As in a file explorer: node is copied (see CopyNode) and marked as cut, but stays in place until pasted in this
		// prefab, which moves it (see PasteNode). Cancelled by CancelCut (Escape), another copy, or the clipboard changing.
		// @return false for the prefab root, which can't be moved
		bool			CutNode(ui2::Node* node);
		void			CancelCut();
		// Whether node, or one of its ancestors, is marked as cut (see CutNode).
		bool			IsCut(const ui2::Node* node) const;
		// Whether the clipboard holds a copied node.
		bool			CanPasteNode() const;
		// Pastes the clipboard's node, selected, at index among parent's children (the last place if out of range), with new
		// local ids (see ui2::Node::RenewLocalIds). The references its Interactions hold to nodes outside of the copied subtree
		// are kept when pasted in the prefab it was copied from, dropped otherwise. A node cut in this prefab (see CutNode) is
		// moved there instead, keeping its ids, unless parent is that node or one of its descendants (nothing is done then).
		// Same contract as ConvertNode: defer the call to after the drawing pass.
		// @return the pasted (or moved) node, nullptr if nothing was pasted
		ui2::Node*		PasteNode(ui2::Node* parent, uint32_t index = UINT32_MAX);
		// Pastes right after the selected node among its siblings (to chain pastes), or as the last child of the prefab root
		// when it is the selection (or there is none).
		ui2::Node*		PasteNodeNextToSelection();
		// Pastes as the last child of the selected node, or of the prefab root when there is no selection.
		ui2::Node*		PasteNodeInSelection();
		// Places a copy of node right after it among its siblings, selected, without going through the clipboard. Same contract
		// as ConvertNode.
		// @return the copy, nullptr for the prefab root, which can't have siblings
		ui2::Node*		DuplicateNode(ui2::Node* node);
		// Ctrl+C / Ctrl+X / Ctrl+V (next to the selection) / Ctrl+Shift+V (in the selection) / Ctrl+D on the selection, Escape
		// cancelling a cut, for the focused window calling it. Same contract as ConvertNode.
		void			HandleClipboardShortcuts();
		// Gives a local id to the nodes that have none yet (they are otherwise given on save): for something to reference
		// them right away, e.g. a property override's target.
		void			AssignLocalIds();

		// Record mode: the prefab shows a Button's state (its overrides applied), and editing the nodes' overridable properties
		// (theirs, their LayoutParams' and Layout's: see ui2::PropertyValue) records the state's overrides instead of changing the
		// prefab: the state's overrides are kept equal to what differs from the prefab as authored, until StopRecording() puts
		// that back. Other edits (nodes added, removed, moved, layout type changed...) still change the prefab itself. Saving while recording saves the prefab as
		// authored, plus the overrides recorded so far, and keeps recording.
		bool			IsRecording() const;
		bool			IsRecording(const ui2::Node* buttonNode, ui2::Button::State state) const;
		const char*		GetRecordedStateName() const;
		void			StartRecording(ui2::Node* buttonNode, ui2::Button::State state);
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

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		void	LoadNodes();

		// Guarantees the canvas' root node holds exactly one child (the prefab root): creates it for an
		// empty prefab, or, for one saved when the canvas' root node itself was the prefab root (possibly
		// with several children), wraps those children in a stretched prefab root so they keep their
		// placement.
		void	EnsurePrefabRoot();

		void	GenerateThumbnail();

		// Clipboard (see CopyNode)
		bool		SerializeNodeAsAuthored(ui2::Node* node, DocumentNode& documentNode);
		ui2::Node*	InsertNodeCopy(ui2::Node* parent, uint32_t index, const DocumentNode& documentNode, bool keepOutsideTargets);
		ui2::Node*	MoveNode(ui2::Node* node, ui2::Node* parent, uint32_t index);

		// Record mode (see StartRecording)
		ui2::Button*	FindRecordedButton() const;
		void			UpdateRecording();
		void			RestoreRecordedBase();
		void			ApplyRecordedOverrides();

	private:

		ui2::Canvas	_canvas;
		ui2::Node*	_selectedNode = nullptr;
		bool		_canvasSelected = false;
		bool		_frameSelectionRequested = false;

		// Counter the nodes' local ids are taken from (see ui2::Node::GetLocalId). Persisted next to the
		// nodes in the asset's document: it belongs to the authored prefab, not to the canvas.
		uint64_t	_nextLocalId = 1;

		// Record mode: the recorded Button's node (local id, 0 when not recording), its state, and the overridable properties
		// of every node as authored when recording started
		struct RecordedBaseValue
		{
			uint64_t			_target = 0;
			String				_property;
			ui2::PropertyValue	_value;
		};
		uint64_t					_recordedNode = 0;
		ui2::Button::State			_recordedState = ui2::Button::State::Normal;
		Vector<RecordedBaseValue>	_recordedBase;

		ui2::Canvas*					_interactionCanvas = nullptr; // Play mode's copy, null when not playing
		bool							_paused = false;
		bool							_nextFrameRequested = false;
		ui2::Canvas::ActionEvent::Slot	_onInteractionAction;
		Vector<InteractionLogEntry>		_interactionLog;

		// Cut (see CutNode): the cut node's local id (0 when none), and the clipboard's text the cut put there, which must still
		// be the clipboard's for a paste to move the node
		uint64_t	_cutNode = 0;
		String		_cutClipboardText;
	};
}

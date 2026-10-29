#pragma once
#include "HodEngine/UI2Editor/Export.hpp"
#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/UI2/Button.hpp>
#include <HodEngine/UI2/Canvas.hpp>
#include <HodEngine/UI2/PropertyOverride.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
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

		// Gives a local id to the nodes that have none yet (they are otherwise given on save): for something to reference
		// them right away, e.g. a property override's target.
		void			AssignLocalIds();

		// Record mode: the prefab shows a Button's state (its overrides applied), and editing the nodes' overridable properties
		// (colors, scales...: see ui2::PropertyValue) records the state's overrides instead of changing the prefab: the state's
		// overrides are kept equal to what differs from the prefab as authored, until StopRecording() puts that back. Other
		// edits (texts, placement, nodes added...) still change the prefab itself. Saving while recording saves the prefab as
		// authored, plus the overrides recorded so far, and keeps recording.
		bool			IsRecording() const;
		bool			IsRecording(const ui2::Node* buttonNode, ui2::Button::State state) const;
		const char*		GetRecordedStateName() const;
		void			StartRecording(ui2::Node* buttonNode, ui2::Button::State state);
		void			StopRecording();

		// While recording, the override of that property is removed by putting the property back to its authored value.
		void			RevertRecordedProperty(uint64_t target, const String& property);

		// Interact mode (toggled from the viewport's toolbar): the viewport's pointer drives the prefab's Interactions, to check
		// hit-tests and state changes. Played on a copy of the canvas, made when entering it and dropped when leaving it, so
		// nothing the Interactions change ends up in the prefab. Editing is suspended meanwhile.
		bool			IsInteracting() const;
		void			SetInteracting(bool interacting);

		// The canvas the viewport shows: the interact mode's copy while interacting, the edited canvas otherwise.
		ui2::Canvas&	GetDisplayedCanvas();
		ui2::Node*		GetDisplayedPrefabRoot() const;

		// The actions the interact mode's canvas emitted (see ui2::Canvas::GetActionEvent), most recent last. Cleared on entering it.
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

		ui2::Canvas*					_interactionCanvas = nullptr; // Interact mode's copy, null when not interacting
		ui2::Canvas::ActionEvent::Slot	_onInteractionAction;
		Vector<InteractionLogEntry>		_interactionLog;
	};
}

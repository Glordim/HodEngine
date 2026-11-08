#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <memory>
#include <string_view>

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Editor/AssetContainer.hpp>
#include <HodEngine/Editor/UndoHistory.hpp>

namespace hod::inline core
{
	class Document;
}

namespace hod::inline editor
{
	class Asset;
	class EditorTabWindow;

	class HOD_EDITOR_API EditorTab
	{
	public:
		EditorTab(std::shared_ptr<Asset> asset, const char* icon = nullptr);
		virtual ~EditorTab();

		bool                   Draw();
		std::shared_ptr<Asset> GetAsset() const;

		bool IsDirty() const;
		// An edit was made. With undo (see CaptureUndoState), it becomes an undo step once over: at the end of the frame no widget
		// is active nor the mouse's left button down, so a whole drag or text input makes one step. undoLabel names it (e.g.
		// "Rename", shown as "Undo Rename"), the last one given before the step is made wins.
		void MarkAsDirty(std::string_view undoLabel = {});
		bool Save();

		// Save As: the tab's content written as a new asset (new UID, the asset it was opened from left as last saved), which the
		// tab then edits. RequestSaveAs asks where (a modal, see DrawSaveAs), SaveAs writes it.
		bool CanSaveAs() const;
		void RequestSaveAs();
		bool SaveAs(const Path& path);

		// Undo/redo, for the tabs implementing CaptureUndoState: each tab has its own history (see UndoHistory)
		bool          CanUndo() const;
		bool          CanRedo() const;
		const String& GetUndoLabel() const;
		const String& GetRedoLabel() const;
		bool          Undo();
		bool          Redo();

		// Whether the tab is the one shown in the tab bar (the last frame it was drawn)
		bool IsSelected() const;

		template<typename _EditorTabWindow_>
		_EditorTabWindow_* OpenWindow();

		void Focus();

	protected:
		virtual void CreateDefaultLayout() = 0;
		virtual void DrawMenuBar() = 0;
		// The popups opened from the menu bar, drawn right after it but with the default style (the menu bar compacts its widgets)
		virtual void DrawMenuBarPopups() {}
		virtual bool DrawContent() = 0;

		/// @brief Serializes this tab's edited data to its Asset. Overridden by tabs that hold savable data.
		virtual bool OnSave(); // TODO make it pure

		/// @brief Undo: writes the whole edited state (what an undo step gives back) to document. Not overridden (returns false), the
		/// tab has no undo. Must give the same document for the same state, the steps being compared to tell an edit from nothing.
		virtual bool CaptureUndoState(Document& document);
		/// @brief Gives the tab back the state document holds (see CaptureUndoState).
		virtual bool RestoreUndoState(const Document& document);
		/// @brief Whether undo and redo can be used now (e.g. not in a play mode where editing is suspended)
		virtual bool IsUndoAvailable() const;

	protected:
		String  _title;
		ImGuiID _dockSpaceId;

		AssetContainer _assetContainer;

	private:
		// Undo (see MarkAsDirty)
		void InitUndoHistory();
		void CommitUndoStep();
		bool RestoreUndoStep(const String* state);
		void HandleUndoShortcuts();
		void HandleSaveShortcut();
		bool DrawCloseConfirmation();
		void DrawSaveAs();

	private:
		bool _initialLayoutCreated = false;
		bool _dirty = false;
		bool _editPrepared = false; // VersionControl::PrepareEdit called, ReleaseEdit when closed
		bool _selected = false;
		bool _closeRequested = false; // Closed with unsaved changes: Save / Don't Save / Cancel asked first
		bool _closeConfirmed = false; // Saved or discarded in the modal: closes at the start of the next Draw

		// Save As (see RequestSaveAs)
		bool _saveAsRequested = false;
		bool _closeAfterSaveAs = false; // Save asked by the close confirmation of an untitled tab
		Path _saveAsFolder;
		char _saveAsName[256] = {};

		String _icon; // Of the title, rebuilt when the asset changes (see SaveAs)

		UndoHistory _undoHistory;
		bool        _undoInitialized = false;
		bool        _undoEnabled = false; // CaptureUndoState is implemented
		bool        _undoCommitPending = false;
		String      _pendingUndoLabel;

		std::shared_ptr<Asset> _asset;

		Vector<EditorTabWindow*> _windows;

		bool _focus = false;
	};

	template<typename _EditorTabWindow_>
	_EditorTabWindow_* EditorTab::OpenWindow()
	{
		_EditorTabWindow_* window = DefaultAllocator::GetInstance().New<_EditorTabWindow_>(this);
		_windows.push_back(window);
		return window;
	}
}

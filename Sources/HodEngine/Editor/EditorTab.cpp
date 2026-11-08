#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/AssetType.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/VersionControl/VersionControl.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/Document/DocumentWriterJson.hpp>

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/DearImGui/imgui_internal.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

namespace hod::inline editor
{
	/// @brief
	/// @param asset
	EditorTab::EditorTab(std::shared_ptr<Asset> asset, const char* icon)
	{
		_asset = asset;
		_icon = icon;
		if (_asset == nullptr)
		{
			_assetContainer.SetUid(UID::GenerateUID());
			_title = fmt::format("{}   Untitled", icon);
		}
		else
		{
			_assetContainer.Load(asset->GetPath());
			_title = fmt::format("{}   {}", icon, _asset->GetName());
		}

		_dockSpaceId = ImGui::GetID(this);
	}

	/// @brief
	EditorTab::~EditorTab()
	{
		// Its checkouts made for nothing (closed without saving, or saved unchanged) are released
		if (_editPrepared && _asset != nullptr && VersionControl::GetInstance() != nullptr)
		{
			VersionControl::GetInstance()->ReleaseEdit(VersionControl::GetAssetFiles(*_asset));
		}

		for (EditorTabWindow* window : _windows)
		{
			DefaultAllocator::GetInstance().Delete(window);
		}
	}

	/// @brief
	/// @return
	bool EditorTab::Draw()
	{
		// Close confirmed in the previous frame: nothing drawn this one, what was drawn then (render commands using its render
		// targets) is rendered by now
		if (_closeConfirmed)
		{
			return false;
		}

		// Before anything is drawn, as the undo shortcuts below: what the windows drew in the previous frame is rendered by now,
		// while a restored step replaces what they hold. The edits of the previous frame are over if no drag nor text input went
		// on since (see MarkAsDirty).
		if (_undoCommitPending && ImGui::IsAnyItemActive() == false && ImGui::IsMouseDown(ImGuiMouseButton_Left) == false)
		{
			CommitUndoStep();
		}

		bool open = true;
		ImGui::SetNextItemWidth(210);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
		ImGuiTabItemFlags tabItemFlags = ImGuiTabItemFlags_None;
		if (IsDirty())
		{
			tabItemFlags |= ImGuiTabItemFlags_UnsavedDocument;
		}
		if (_focus)
		{
			_focus = false;
			tabItemFlags |= ImGuiTabItemFlags_SetSelected;
		}
		// "###": the tab keeps its id when its title changes (see SaveAs)
		const String label(fmt::format("{}###EditorTab{}", _title, static_cast<const void*>(this)));
		bool         draw = ImGui::BeginTabItem(label.CStr(), &open, tabItemFlags);
		ImGui::PopStyleVar();
		if (open == false && IsDirty())
		{
			// Its close button (or a middle click): kept until confirmed (see DrawCloseConfirmation)
			open = true;
			_closeRequested = true;
		}
		_selected = draw && open;
		if (draw && open) // check open to avoid crash when RT owner is deleted after push rendercommand using it
		{
			InitUndoHistory();
			HandleUndoShortcuts();
			HandleSaveShortcut();

			float y = ImGui::GetCursorScreenPos().y - ImGui::GetStyle().ItemSpacing.y;
			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(ImGui::GetMainViewport()->Pos.x, y), ImVec2(ImGui::GetMainViewport()->Pos.x + ImGui::GetMainViewport()->Size.x, y + 32), ImGui::GetColorU32(ImGuiCol_TabSelected));

			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
			ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
			ImGui::PushStyleVarX(ImGuiStyleVar_FramePadding, 12.0f);

			ImVec2 cursorPos = ImGui::GetCursorScreenPos();
			cursorPos.x = ImGui::GetMainViewport()->Pos.x;
			cursorPos.y += 32.0f;

			ImGui::SetCursorScreenPos(ImVec2(ImGui::GetMainViewport()->Pos.x, y));
			ImGui::BeginDisabled(_dirty == false);
			if (ImGui::Button(ICON_MDI_CONTENT_SAVE, ImVec2(0.0f, 32.0f)))
			{
				if (_asset)
				{
					Save();
				}
				else
				{
					RequestSaveAs();
				}
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::BeginDisabled(_asset == nullptr);
			if (ImGui::Button(ICON_MDI_MAGNIFY, ImVec2(0.0f, 32.0f)))
			{
				Editor::GetInstance()->PingAsset(_asset);
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical, 2.0f);
			DrawMenuBar();

			ImGui::PopStyleVar(3);
			ImGui::PopStyleColor();
			DrawMenuBarPopups();
			ImGui::SetCursorScreenPos(cursorPos);

			if (_initialLayoutCreated == false)
			{
				CreateDefaultLayout();
				_initialLayoutCreated = true;
			}
			imgui::Window::PushDockTabStyle();
			ImGui::DockSpace(_dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
			imgui::Window::PopDockTabStyle();
			auto it = _windows.Begin();
			auto itEnd = _windows.End();
			while (it != itEnd)
			{
				(*it)->Draw();
				if ((*it)->IsClosed())
				{
					DefaultAllocator::GetInstance().Delete(*it);
					_windows.Erase(it);
					itEnd = _windows.End();
				}
				else
				{
					++it;
				}
			}

			if (DrawContent() == false)
			{
				open = false;
			}
		}
		if (draw)
		{
			ImGui::EndTabItem();
		}

		// Out of the tab item: it may not be the tab shown
		// Closed in the next frame: the tab content drawn in this one pushed render commands using its render targets
		if (_closeRequested && DrawCloseConfirmation())
		{
			_closeConfirmed = true;
		}
		if (_saveAsRequested)
		{
			DrawSaveAs();
		}
		return open;
	}

	/// @brief The modal asking what to do with the unsaved changes of the tab being closed
	/// @return True when the tab can close now (saved, or changes discarded)
	bool EditorTab::DrawCloseConfirmation()
	{
		constexpr const char* ModalName = "Unsaved changes###EditorTabCloseConfirmation";

		bool close = false;
		ImGui::PushID(this);
		if (ImGui::IsPopupOpen(ModalName) == false)
		{
			ImGui::OpenPopup(ModalName);
		}
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(ModalName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			const String name = _asset != nullptr ? _asset->GetName() : String("Untitled");
			ImGui::Text("%s has unsaved changes.", name.CStr());
			ImGui::TextUnformatted("Save them before closing?");
			ImGui::Spacing();

			const bool canSave = _asset != nullptr || CanSaveAs();
			ImGui::BeginDisabled(canSave == false);
			if (ImGui::Button(ICON_MDI_CONTENT_SAVE " Save") || (canSave && ImGui::IsKeyPressed(ImGuiKey_Enter)))
			{
				if (_asset != nullptr)
				{
					close = Save(); // Failed: stays open, with its changes (the error is in the output log)
				}
				else
				{
					// Closed once saved where the Save As modal says (see DrawSaveAs)
					RequestSaveAs();
					_closeAfterSaveAs = true;
				}
				_closeRequested = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndDisabled();
			ImGui::SameLine();
			if (ImGui::Button("Don't Save"))
			{
				close = true;
				_closeRequested = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
			{
				_closeRequested = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
		return close;
	}

	/// @brief Its type is known: the extension of the new file, and what is done when the asset gets another name
	bool EditorTab::CanSaveAs() const
	{
		return AssetDatabase::GetInstance()->FindAssetType(_assetContainer.GetAssetType()) != nullptr;
	}

	/// @brief Opens the Save As modal (drawn by Draw), in the folder of the asset with its name
	void EditorTab::RequestSaveAs()
	{
		if (CanSaveAs() == false)
		{
			return;
		}

		_saveAsRequested = true;
		const String name = _asset != nullptr ? _asset->GetName() : String("Untitled");
		std::snprintf(_saveAsName, sizeof(_saveAsName), "%s", name.CStr());
		_saveAsFolder = _asset != nullptr ? _asset->GetPath().ParentPath() : AssetDatabase::GetInstance()->GetAssetRootNode()._path;
	}

	/// @brief
	/// @param path A file that doesn't exist yet, in a folder of the asset database
	/// @return false (with an error) if it couldn't be written: the tab is left as it was, with its changes
	bool EditorTab::SaveAs(const Path& path)
	{
		const AssetType* assetType = AssetDatabase::GetInstance()->FindAssetType(_assetContainer.GetAssetType());
		if (assetType == nullptr)
		{
			OUTPUT_ERROR("Can't save {} as {}: its asset type isn't registered", _title, path);
			return false;
		}
		if (FileSystem::GetInstance()->Exists(path))
		{
			OUTPUT_ERROR("Can't save {} as {}: the file exists already", _title, path);
			return false;
		}

		// The edit in progress, if any, is what gets saved (see Save)
		if (_undoCommitPending)
		{
			CommitUndoStep();
		}

		if (OnSave() == false)
		{
			return false;
		}

		// A new asset: the references to the one it was opened from stay on it
		const UID previousUid = _assetContainer.GetUid();
		_assetContainer.SetUid(UID::GenerateUID());
		if (_assetContainer.Save(path) == false)
		{
			// Save clears the data blocks, restored as in Save
			_assetContainer = AssetContainer();
			if (_asset != nullptr)
			{
				_assetContainer.Load(_asset->GetPath());
			}
			else
			{
				_assetContainer.SetUid(previousUid);
				_assetContainer.SetAssetType(assetType->GetHashV());
			}
			return false;
		}

		std::shared_ptr<Asset> newAsset = AssetDatabase::GetInstance()->AddWrittenAsset(path);
		if (newAsset == nullptr)
		{
			OUTPUT_ERROR("Can't save {} as {}: written, but not loadable as an asset", _title, path);
			return false;
		}
		// The content was saved with the name of the asset it came from (a UI prefab's root...)
		if (_asset == nullptr || newAsset->GetName() != _asset->GetName())
		{
			assetType->OnRenamed(*newAsset);
		}

		// The checkout of the asset it was opened from was made for nothing: it is left as last saved
		if (_editPrepared && _asset != nullptr && VersionControl::GetInstance() != nullptr)
		{
			VersionControl::GetInstance()->ReleaseEdit(VersionControl::GetAssetFiles(*_asset));
		}
		_editPrepared = false;

		_asset = newAsset;
		_title = fmt::format("{}   {}", _icon, _asset->GetName());
		_dirty = false;
		_undoHistory.MarkSaved();
		return true;
	}

	/// @brief Of the Save As modal: a folder of the asset database and its tree nodes below, the selected one opened on appearing
	static void DrawSaveAsFolderNode(const AssetDatabase::FileSystemMapping& node, const AssetDatabase::FileSystemMapping* selectedNode, bool reveal, Path& selectedFolder)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (node._childrenFolder.Empty())
		{
			flags |= ImGuiTreeNodeFlags_Leaf;
		}
		if (node._parentFolder == nullptr)
		{
			flags |= ImGuiTreeNodeFlags_DefaultOpen;
		}
		if (&node == selectedNode)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}
		if (reveal && selectedNode != nullptr)
		{
			for (const AssetDatabase::FileSystemMapping* ancestor = selectedNode->_parentFolder; ancestor != nullptr; ancestor = ancestor->_parentFolder)
			{
				if (ancestor == &node)
				{
					ImGui::SetNextItemOpen(true);
					break;
				}
			}
		}

		const String name = node._path.Filename().GetString();
		ImGui::PushID(name.CStr());
		const bool opened = ImGui::TreeNodeEx("##Folder", flags, ICON_MDI_FOLDER " %s", name.CStr());
		if (ImGui::IsItemClicked() && ImGui::IsItemToggledOpen() == false)
		{
			selectedFolder = node._path;
		}
		if (reveal && &node == selectedNode)
		{
			ImGui::SetScrollHereY();
		}
		if (opened)
		{
			for (const AssetDatabase::FileSystemMapping* child : node._childrenFolder)
			{
				DrawSaveAsFolderNode(*child, selectedNode, reveal, selectedFolder);
			}
			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	/// @brief The modal of RequestSaveAs: a folder of the asset database and a name, of a file that doesn't exist yet
	void EditorTab::DrawSaveAs()
	{
		constexpr const char* ModalName = "Save As###EditorTabSaveAs";

		const AssetType* assetType = AssetDatabase::GetInstance()->FindAssetType(_assetContainer.GetAssetType());
		bool             done = false;
		ImGui::PushID(this);
		if (ImGui::IsPopupOpen(ModalName) == false)
		{
			ImGui::OpenPopup(ModalName);
		}
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal(ModalName, nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (assetType == nullptr) // Its module unloaded meanwhile
			{
				done = true;
			}
			else
			{
				const bool appearing = ImGui::IsWindowAppearing();

				// The folder may have been removed meanwhile (file watcher): back to the root then
				AssetDatabase::FileSystemMapping* selectedNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(_saveAsFolder);
				if (selectedNode == nullptr || selectedNode->_type != AssetDatabase::FileSystemMapping::Type::FolderType)
				{
					selectedNode = &AssetDatabase::GetInstance()->GetAssetRootNode();
					_saveAsFolder = selectedNode->_path;
				}

				ImGui::TextUnformatted("Folder");
				if (ImGui::BeginChild("Folders", ImVec2(420.0f, 260.0f), ImGuiChildFlags_Borders))
				{
					DrawSaveAsFolderNode(AssetDatabase::GetInstance()->GetAssetRootNode(), selectedNode, appearing, _saveAsFolder);
				}
				ImGui::EndChild();

				ImGui::Spacing();
				ImGui::TextUnformatted("Name");
				if (appearing)
				{
					ImGui::SetKeyboardFocusHere();
				}
				String extension = ".";
				extension += assetType->GetExtensionV();
				ImGui::SetNextItemWidth(420.0f - ImGui::CalcTextSize(extension.CStr()).x - ImGui::GetStyle().ItemSpacing.x);
				const bool submitted = ImGui::InputText("##Name", _saveAsName, sizeof(_saveAsName), ImGuiInputTextFlags_EnterReturnsTrue);
				ImGui::SameLine();
				ImGui::TextDisabled("%s", extension.CStr());

				// What prevents saving, shown under the name
				std::string_view name(_saveAsName);
				const char*      error = nullptr;
				Path             path;
				if (name.find_first_not_of(' ') == std::string_view::npos)
				{
					error = "A name is needed";
				}
				else if (name.find_first_of("\\/:*?\"<>|") != std::string_view::npos)
				{
					error = "A name can't contain \\ / : * ? \" < > |";
				}
				else if (name.front() == ' ' || name.back() == ' ' || name.back() == '.')
				{
					error = "A name can't start with a space nor end with a space or a dot";
				}
				else
				{
					path = _saveAsFolder / AssetType::MakeFileName(name, assetType->GetExtensionV());
					if (FileSystem::GetInstance()->Exists(path))
					{
						error = "A file of this name exists in this folder";
					}
				}
				if (error != nullptr)
				{
					ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", error);
				}
				else
				{
					ImGui::TextDisabled("%s", path.GetString().CStr());
				}

				ImGui::Spacing();
				ImGui::BeginDisabled(error != nullptr);
				if (ImGui::Button(ICON_MDI_CONTENT_SAVE " Save") || (submitted && error == nullptr))
				{
					// Failed: the modal stays, the error is in the output log
					if (SaveAs(path))
					{
						if (_closeAfterSaveAs)
						{
							_closeConfirmed = true; // Closed at the start of the next Draw (see DrawCloseConfirmation)
						}
						done = true;
					}
				}
				ImGui::EndDisabled();
				ImGui::SameLine();
				if (ImGui::Button("Cancel") || ImGui::IsKeyPressed(ImGuiKey_Escape))
				{
					done = true;
				}
			}

			if (done)
			{
				_saveAsRequested = false;
				_closeAfterSaveAs = false;
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}
		ImGui::PopID();
	}

	/// @brief
	/// @return
	std::shared_ptr<Asset> EditorTab::GetAsset() const
	{
		return _asset;
	}

	/// @brief
	/// @return
	bool EditorTab::IsDirty() const
	{
		return _dirty;
	}

	/// @brief
	/// @param undoLabel
	void EditorTab::MarkAsDirty(std::string_view undoLabel)
	{
		// The first change: checked out now rather than on save, a lock taken meanwhile is known right away
		if (_dirty == false && _asset != nullptr && VersionControl::GetInstance() != nullptr)
		{
			VersionControl::GetInstance()->PrepareEdit(VersionControl::GetAssetFiles(*_asset));
			_editPrepared = true;
		}

		_dirty = true;
		_undoCommitPending = true;
		if (undoLabel.empty() == false)
		{
			_pendingUndoLabel = undoLabel;
		}
	}

	/// @brief
	/// @return
	bool EditorTab::Save()
	{
		// The edit in progress, if any, is what gets saved: it is the saved step
		if (_undoCommitPending)
		{
			CommitUndoStep();
		}

		if (_dirty == false)
		{
			return true;
		}

		if (OnSave() == false)
		{
			return false;
		}

		if (_assetContainer.Save(_asset->GetPath()) == false)
		{
			_assetContainer = AssetContainer();
			_assetContainer.Load(_asset->GetPath()); // Save clear all datablock before moving tmp to dst, if save fail we re-load to restore it
			return false;
		}

		_dirty = false;
		_undoHistory.MarkSaved();
		return true;
	}

	/// @brief
	/// @return
	bool EditorTab::OnSave()
	{
		return false;
	}

	/// @brief
	/// @param document
	/// @return
	bool EditorTab::CaptureUndoState(Document& document)
	{
		(void)document;
		return false;
	}

	/// @brief
	/// @param document
	/// @return
	bool EditorTab::RestoreUndoState(const Document& document)
	{
		(void)document;
		return false;
	}

	/// @brief
	/// @return
	bool EditorTab::IsUndoAvailable() const
	{
		return true;
	}

	namespace
	{
		/// @brief The state document holds, as an undo step keeps it
		bool WriteUndoState(Document& document, String& outState)
		{
			DocumentWriterJson documentWriter;
			return documentWriter.Write(document, outState);
		}
	}

	/// @brief The first step: the state the tab was opened with. Lazily, on the first frame drawn: the virtual CaptureUndoState can't
	/// be called from the constructor.
	void EditorTab::InitUndoHistory()
	{
		if (_undoInitialized)
		{
			return;
		}
		_undoInitialized = true;

		Document document;
		String   state;
		_undoEnabled = CaptureUndoState(document) && WriteUndoState(document, state);
		if (_undoEnabled)
		{
			_undoHistory.Reset(std::move(state));
		}
	}

	/// @brief Makes the edits since the last step one step (see MarkAsDirty). The tab is clean again if they gave back the saved
	/// state (e.g. a drag cancelled, a value typed back).
	void EditorTab::CommitUndoStep()
	{
		_undoCommitPending = false;
		String label = std::move(_pendingUndoLabel);
		_pendingUndoLabel.Clear();

		if (_undoInitialized == false || _undoEnabled == false)
		{
			return;
		}

		Document document;
		String   state;
		if (CaptureUndoState(document) == false || WriteUndoState(document, state) == false)
		{
			return;
		}
		_undoCommitPending = false; // what CaptureUndoState itself marked (e.g. ids given) is in the step
		_undoHistory.Commit(std::move(label), std::move(state));
		_dirty = (_undoHistory.IsAtSavedStep() == false);
	}

	/// @brief
	/// @return
	bool EditorTab::CanUndo() const
	{
		return _undoEnabled && IsUndoAvailable() && (_undoHistory.CanUndo() || _undoCommitPending);
	}

	/// @brief
	/// @return
	bool EditorTab::CanRedo() const
	{
		return _undoEnabled && IsUndoAvailable() && _undoHistory.CanRedo() && _undoCommitPending == false;
	}

	/// @brief
	/// @return
	const String& EditorTab::GetUndoLabel() const
	{
		return _undoCommitPending ? _pendingUndoLabel : _undoHistory.GetUndoLabel();
	}

	/// @brief
	/// @return
	const String& EditorTab::GetRedoLabel() const
	{
		return _undoHistory.GetRedoLabel();
	}

	/// @brief
	/// @return
	bool EditorTab::Undo()
	{
		if (CanUndo() == false)
		{
			return false;
		}
		// The edit in progress first becomes a step, to be the one undone
		if (_undoCommitPending)
		{
			CommitUndoStep();
		}
		return RestoreUndoStep(_undoHistory.Undo());
	}

	/// @brief
	/// @return
	bool EditorTab::Redo()
	{
		if (CanRedo() == false)
		{
			return false;
		}
		return RestoreUndoStep(_undoHistory.Redo());
	}

	/// @brief
	/// @param state
	/// @return
	bool EditorTab::RestoreUndoStep(const String* state)
	{
		if (state == nullptr)
		{
			return false;
		}

		Document           document;
		DocumentReaderJson documentReader;
		if (documentReader.Read(document, state->CStr(), state->Length()) == false || RestoreUndoState(document) == false)
		{
			OUTPUT_ERROR("Can't restore the undo step of {}", _title);
			return false;
		}
		// What the restoration marked isn't an edit
		_undoCommitPending = false;
		_pendingUndoLabel.Clear();
		_dirty = (_undoHistory.IsAtSavedStep() == false);
		return true;
	}

	/// @brief Ctrl+S, from anywhere in the editor (its windows, the asset browser...): saves the tab shown, the only one drawing
	void EditorTab::HandleSaveShortcut()
	{
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteGlobal) && _dirty)
		{
			if (_asset != nullptr)
			{
				Save();
			}
			else
			{
				RequestSaveAs();
			}
		}
		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_S, ImGuiInputFlags_RouteGlobal))
		{
			RequestSaveAs();
		}
	}

	/// @brief Ctrl+Z, Ctrl+Y / Ctrl+Shift+Z, held to repeat. Left to the text field being edited, and not in the middle of an edit
	/// (a drag going on).
	void EditorTab::HandleUndoShortcuts()
	{
		if (ImGui::GetIO().WantTextInput || ImGui::IsAnyItemActive() || ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			return;
		}

		if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_Repeat))
		{
			Undo();
		}
		else if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_Repeat) ||
		         ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal | ImGuiInputFlags_Repeat))
		{
			Redo();
		}
	}

	/// @brief
	/// @return
	bool EditorTab::IsSelected() const
	{
		return _selected;
	}

	/// @brief
	void EditorTab::Focus()
	{
		_focus = true;
	}
}

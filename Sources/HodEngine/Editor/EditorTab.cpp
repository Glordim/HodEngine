#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AssetContainer.hpp"
#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include "HodEngine/Editor/Asset.hpp"
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
		bool draw = ImGui::BeginTabItem(_title.CStr(), &open, tabItemFlags);
		ImGui::PopStyleVar();
		_selected = draw && open;
		if (draw && open) // check open to avoid crash when RT owner is deleted after push rendercommand using it
		{
			InitUndoHistory();
			HandleUndoShortcuts();

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
					// TODO SaveAs
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
		return open;
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

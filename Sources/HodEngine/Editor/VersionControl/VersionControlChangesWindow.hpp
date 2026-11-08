#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/VersionControl/VersionControl.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <memory>
#include <unordered_map>

namespace hod::inline editor
{
	/// @brief The changes of the project (VersionControl::GetChanges), the ones checked submitted with a message (Commit, Commit & Push
	/// with Git; Submit with the others), and the push of the local commits. Window > Version Control Changes, the asset browser's
	/// toolbar, "Version Control/Submit..." on a selection
	class HOD_EDITOR_API VersionControlChangesWindow : public imgui::Window
	{
		REFLECTED_CLASS(VersionControlChangesWindow, imgui::Window)

	public:
		/// @brief Opens the window, or focuses it when already open
		static void Open();
		/// @brief Same, with only the changes under these paths checked
		static void Open(const Vector<Path>& only);

	public:
		VersionControlChangesWindow();

		void DrawContent() override;

	private:
		/// @brief Of a submit, shared with its callback: the window may be closed before it's done
		struct SubmitState
		{
			bool   _running = false;
			bool   _done = false;
			bool   _success = false;
			String _error;
		};

		using Change = VersionControl::Change;

		void DrawHeader();
		void DrawChanges();
		void DrawChangeContextMenu(const Change& change);
		void DrawSubmit();
		void DrawSubmitContent();

		void   RefreshChanges();
		bool   IsChecked(const Change& change) const;
		bool   IsSubmittable(const Change& change) const;
		void   SetChecked(const Change& change, bool checked);
		String GetDisplayPath(const Path& path) const;

	private:
		Vector<Change>                 _changes; // Without the original paths of the renamed ones (submitted with them)
		uint32_t                       _statusVersion = 0;
		bool                           _changesLoaded = false;
		std::unordered_map<Path, bool> _checked; // The user's choices, by default (IsChecked): the tracked ones, or the ones under _only
		Vector<Path>                   _only;

		char                         _message[8192] = {'\0'};
		std::shared_ptr<SubmitState> _submit = std::make_shared<SubmitState>();
		float                        _submitHeight = 0.0f; // Measured on the previous frame (its error/warning lines come and go), the table takes the rest
	};
}

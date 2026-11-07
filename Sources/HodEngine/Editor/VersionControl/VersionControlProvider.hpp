#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/VersionControl/VersionControlStatus.hpp"

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <cstdint>
#include <string_view>
#include <unordered_map>

namespace hod::inline editor
{
	/// @brief A version control system (Git, Svn, Perforce...), through its command line (see Process::Run): the user's config
	/// and credentials are used as they are. Registered by an editor module (EditorModuleRegistration::RegisterVersionControlProvider),
	/// used through VersionControl only.
	///
	/// Apart from GetName/Detect, its functions are called on the worker thread of VersionControl, one at a time: no locking
	/// needed inside, and they may block (a command line, the network). Never interactive: no stdin, the prompts are disabled
	/// (GIT_TERMINAL_PROMPT=0, p4 -s...), a missing login is an error of Connect.
	///
	/// Paths are absolute, files or folders (recursive), under the project folder. The batch functions get all the paths at once:
	/// one command line for all of them (or a few, the command line length being limited), not one per path.
	class HOD_EDITOR_API VersionControlProvider
	{
	public:
		/// @brief Flags (GetCapabilities). In their own scope: the names are the ones of the functions
		struct Capability
		{
			enum : uint32_t
			{
				None = 0,
				Checkout = 1 << 0,        // Files read-only until checked out (Perforce, svn:needs-lock, git lfs lockable): VersionControl::PrepareWrite calls Checkout
				Lock = 1 << 1,            // Lock/Unlock (p4 exclusive checkout, svn lock, git lfs lock)
				TrackMoves = 1 << 2,      // Moves and deletions go through Move/Delete (Perforce, Svn). Git detects them on its own
				RemoteStatus = 1 << 3,    // QueryStatus fills _outOfDate (Git: against its upstream, as last fetched)
				GetLatestFiles = 1 << 4,  // GetLatest of some files only (p4 sync, svn update). Not Git: a pull updates the whole working tree
				Diff = 1 << 5,            // OpenDiff
				History = 1 << 6,         // OpenHistory
				UpdateWorkspace = 1 << 7, // UpdateWorkspace (Git: an upstream branch to fast-forward to)
				Submit = 1 << 8,          // Submit (git commit, svn commit, p4 submit)
				Push = 1 << 9,            // Push: Submit is local (Git), sent to the server afterwards (an upstream branch)
			};

			static constexpr uint32_t Count = 10; // Bits used by Capability
		};

		struct HOD_EDITOR_API Result
		{
			bool   _success = true;
			String _error; // The command's error output, for the console

			static Result Failure(std::string_view error);
		};

		using StatusMap = std::unordered_map<Path, VersionControlStatus>;

		/// @brief What SetupWorkspace sets up, given by VersionControl (from the project and the registered asset types and importers)
		struct WorkspaceSetup
		{
			Vector<Path>   _ignoredFolders;   // Cache, Build
			Vector<Path>   _ignoredFiles;     // UserSettings.json, the generated CMakeLists.txt
			Vector<String> _binaryExtensions; // Not mergeable ("texture", "png"...): the asset files and the binary sources, without the dot
		};

	public:
		VersionControlProvider() = default;
		VersionControlProvider(const VersionControlProvider&) = delete;
		VersionControlProvider(VersionControlProvider&&) = delete;
		virtual ~VersionControlProvider() = default;

		VersionControlProvider& operator=(const VersionControlProvider&) = delete;
		VersionControlProvider& operator=(VersionControlProvider&&) = delete;

	public:
		/// @brief "Git", "Svn", "Perforce": shown to the user and stored as their choice
		virtual std::string_view GetName() const = 0;

		/// @brief Main thread, cheap (no command line if possible): whether the project looks under this system (a .git or .svn up
		/// its folders, a P4CONFIG file...), for the automatic choice
		virtual bool Detect(const Path& projectDir) const = 0;

		/// @brief The command line is there, the workspace root, the user (and the p4 client) found, logged in
		virtual Result Connect(const Path& projectDir) = 0;
		virtual void   Disconnect();

		/// @brief After each successful Connect: if the project has no ignore list of this system yet (.gitignore, svn:ignore on its
		/// folder, the file P4IGNORE names...), creates it with setup's ignored paths. Same for what tells the binary files (Git:
		/// .gitattributes, LFS and lockable if git-lfs is there; Svn: svn:needs-lock; Perforce: the typemap is the server's, nothing).
		/// An existing one is left as it is: the team's. Its failure is reported, not fatal. Nothing by default
		virtual Result SetupWorkspace(const Path& projectDir, const WorkspaceSetup& setup);

		/// @brief Capability flags, once connected (after SetupWorkspace): they depend on the workspace (an upstream branch, LFS
		/// lockable files...), and may change from a connection to another
		virtual uint32_t GetCapabilities() const = 0;

		/// @brief Of everything under the folders (the asset, source and project settings folders of the project): files not in
		/// the map are Unchanged (or Unknown if the provider can't tell). Ignored files may be left out. A full refresh: the
		/// provider may also refresh what comes from the server there (git fetch, lock list)
		virtual Result QueryStatus(ArrayView<Path> folders, StatusMap& statuses) = 0;
		/// @brief Of these paths only (folders: what's under them), after an operation or a change on disk. Same convention as
		/// QueryStatus for what's not in the map. Local only if possible (what comes from the server as of the last QueryStatus)
		virtual Result QueryFileStatus(ArrayView<Path> paths, StatusMap& statuses) = 0;

		virtual Result Add(ArrayView<Path> paths) = 0;
		/// @brief Back to their revision on disk (the file watcher reimports them), Added ones untracked again (left on disk), their
		/// checkout and lock released
		virtual Result Revert(ArrayView<Path> paths) = 0;
		/// @brief Releases those of the paths unchanged since their revision (checkout and lock gone, content untouched: p4 revert -a,
		/// svn unlock / git lfs unlock of the unmodified ones). Success by default: nothing to release
		virtual Result RevertUnchanged(ArrayView<Path> paths);

		/// @brief Capability::Checkout: makes them writable, those already checked out (or that don't need it) left as they are.
		/// Success by default: nothing to do
		virtual Result Checkout(ArrayView<Path> paths);

		/// @brief Capability::Lock. Refused if the file changed on the server since its revision (a lock on an out of date file
		/// would be lost work)
		virtual Result Lock(ArrayView<Path> paths);
		virtual Result Unlock(ArrayView<Path> paths);

		/// @brief Capability::GetLatestFiles: their last revision on disk (p4 sync, svn update)
		virtual Result GetLatest(ArrayView<Path> paths);
		/// @brief Capability::UpdateWorkspace: the whole workspace to its last revision (git: fetch + fast-forward merge of the
		/// upstream branch, refused if the branch diverged or if local changes would be overwritten; svn update; p4 sync). folders:
		/// the root folders of the project, for the systems updating per folder. May be long (downloads): its own timeout
		virtual Result UpdateWorkspace(ArrayView<Path> folders);

		/// @brief Capability::Submit: these files only (other changes, staged or not, left as they are), the untracked ones added
		/// and the deleted ones removed. A Renamed file comes with its _originalPath
		virtual Result Submit(ArrayView<Path> paths, std::string_view message);
		/// @brief Capability::Push: the local submits sent to the server (git push), then the locks of the files no longer changed
		/// released (as the server does on a p4 submit)
		virtual Result Push();
		/// @brief Capability::Push: the local submits not sent yet, as of the last QueryStatus/Submit/Push/UpdateWorkspace
		virtual uint32_t GetPendingPushCount() const;

		/// @brief Why a capability is missing, for the disabled actions of the UI ("Git: locks need Git LFS..."). Once connected,
		/// like GetCapabilities. "<Name> can't do it" by default
		virtual String GetCapabilityHint(uint32_t capability) const;
		/// @brief Main thread, cheap: the files of the system in the project folder to show in the changes with the project's
		/// (.gitignore, .gitattributes), whether they exist or not. None by default
		virtual Vector<Path> GetWorkspaceFiles(const Path& projectDir) const;

		/// @brief Capability::TrackMoves, for tracked files only (VersionControl moves the untracked ones on disk itself): moves the
		/// file or folder on disk too (p4 edit + p4 move, svn move)
		virtual Result Move(const Path& from, const Path& to);
		/// @brief Capability::TrackMoves, for tracked files only: deletes them on disk too (p4 delete, svn delete)
		virtual Result Delete(ArrayView<Path> paths);

		/// @brief Capability::Diff: the user's diff tool, detached (git difftool, P4DIFF, svn --diff-cmd), against its revision
		virtual Result OpenDiff(const Path& path);
		/// @brief Capability::History: the user's history tool, detached (gitk / TortoiseGit, p4v -cmd history, TortoiseProc)
		virtual Result OpenHistory(const Path& path);

	protected:
		/// @brief Failure "<Name> can't <operation>", for the functions of a capability it doesn't have
		Result NotSupported(std::string_view operation) const;
	};
}

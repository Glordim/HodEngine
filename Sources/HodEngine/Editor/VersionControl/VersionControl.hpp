#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/VersionControl/VersionControlProvider.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlStatus.hpp"

#include <HodEngine/Core/ArrayView.hpp>
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Thread/Thread.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace hod::inline editor
{
	class Asset;
	class Project;

	/// @brief The version control of the opened project: its provider (one of the registered ones), the statuses of its files
	/// (a cache, refreshed in the background) and the operations on them.
	///
	/// The main thread never waits for a command line, except PrepareWrite/Move/Delete (synchronous by nature). Everything else
	/// is queued to a worker thread running one command at a time (git's index.lock, p4's client): the result comes back as a
	/// refreshed status cache, an entry in the TaskTracker, and an error in the console if it failed.
	///
	/// Only the asset, source and project settings folders of the project are looked at (not Build, Cache...).
	/// <Project>/UserSettings.json is never versioned: left out of the statuses and of every operation (Add, AutoAdd...). Its
	/// ignoring by the system itself is the provider's (VersionControlProvider::SetupWorkspace, on connection).
	class HOD_EDITOR_API VersionControl
	{
		_Singleton(VersionControl)

	public:
		/// @brief Called on the main thread once an operation is done (statuses refreshed already)
		using Callback = std::function<void(const VersionControlProvider::Result& result)>;

		/// @brief Delegated to the provider, or not (no provider, untracked files, no Capability::TrackMoves): the caller does it
		enum class Delegation
		{
			NotHandled,
			Done,
			Failed,
		};

		/// @brief A file of GetChanges
		struct Change
		{
			Path                 _path;
			VersionControlStatus _status;
		};

		/// @brief The name of the file of the per user settings, in the project folder
		static constexpr const char* UserSettingsFileName = "UserSettings.json";

	public:
		~VersionControl();

		/// @brief By Editor when a project is opened/closed: connects the chosen provider (on the worker), then a full refresh.
		/// Closing waits for the operations queued (a ReleaseEdit of the tabs closed...), not for the refreshes
		void OnProjectOpened(const Project& project);
		void OnProjectClosed();

		/// @brief By Editor, each frame on the main thread: the callbacks and errors of the operations done, a refresh when the
		/// editor gets the focus back (what was done outside: a commit, a pull...)
		void Update(bool editorFocused);

		/// @brief By EditorModuleRegistration. Unregistering the connected one disconnects it first (its module unloads), registering
		/// the chosen one while a project is opened connects it (hot reload)
		/// @return false if a provider of the same name is registered already
		bool                                   RegisterProvider(VersionControlProvider& provider);
		void                                   UnregisterProvider(VersionControlProvider& provider);
		const Vector<VersionControlProvider*>& GetProviders() const;

		/// @brief "Auto" (default: the first registered provider detecting the project), "None", or a provider's name. Per user and
		/// per project (differs from a machine to another): in <Project>/UserSettings.json, not a ProjectSettings. Reconnects
		void             SetProviderChoice(std::string_view choice);
		std::string_view GetProviderChoice() const;

		/// @brief nullptr if none or not connected (yet)
		const VersionControlProvider* GetConnectedProvider() const;
		bool                          IsConnected() const;
		/// @brief Why the connection failed (empty otherwise), for the asset browser's toolbar
		String GetConnectionError() const;
		/// @brief Of the connected provider, None otherwise
		uint32_t GetCapabilities() const;
		bool     HasCapability(uint32_t capability) const; // VersionControlProvider::Capability::...
		/// @brief Why the connected provider hasn't this capability (one flag), for the disabled actions. Empty if it has it
		String GetCapabilityHint(uint32_t capability) const;

		// Statuses: from the cache, never blocks. Unknown until refreshed

		VersionControlStatus GetStatus(const Path& path) const;
		/// @brief Of its .asset and of its sources combined (see VersionControlStatus::Combine)
		VersionControlStatus GetAssetStatus(const Asset& asset) const;
		/// @brief Of what's under the folder combined: computed on refresh, not on each call
		VersionControlStatus GetFolderStatus(const Path& folder) const;
		/// @brief Changes each time the cache does
		uint32_t GetStatusVersion() const;
		/// @brief The files of the project with a newer revision on the server (Capability::RemoteStatus), as of the last refresh
		uint32_t GetOutOfDateCount() const;
		/// @brief The local submits not pushed yet (Capability::Push), as of the last refresh or operation
		uint32_t GetPendingPushCount() const;
		/// @brief The files changed (Added, Modified, Deleted, Renamed, Conflicted) or untracked of the project, by path: what the
		/// changes window lists. The project file and the provider's workspace files (.gitignore...) included
		Vector<Change> GetChanges() const;
		/// @brief Whether a refresh or an operation is running or queued (the toolbar's spinner)
		bool IsBusy() const;

		/// @brief The files of an asset, what an operation on it applies to: its .asset and its sources in the project
		static Vector<Path> GetAssetFiles(const Asset& asset);
		/// @brief What the version control handles of the project: the asset, source, project settings and code folders, the
		/// project file (not the provider's workspace files). Empty while no project is open
		const Vector<Path>& GetProjectFolders() const;

		/// @brief The whole project (toolbar's refresh button, focus back to the editor...)
		void RequestRefresh();
		/// @brief These paths only, debounced: by AssetDatabase on the file watcher's events. Any thread
		void RequestRefresh(ArrayView<Path> paths);

		// Operations: queued, asynchronous. Folders are recursive, assets given with all their files (GetAssetFiles)

		void Add(Vector<Path> paths, Callback callback = nullptr);
		void Revert(Vector<Path> paths, Callback callback = nullptr);
		/// @brief By hand: the user's, ReleaseEdit never reverts it
		void Checkout(Vector<Path> paths, Callback callback = nullptr);
		void Lock(Vector<Path> paths, Callback callback = nullptr);
		void Unlock(Vector<Path> paths, Callback callback = nullptr);
		void GetLatest(Vector<Path> paths, Callback callback = nullptr);
		/// @brief Capability::UpdateWorkspace (asset browser's Pull): the whole workspace to its last revision, then a full refresh.
		/// The files changed on disk are reimported by the file watcher
		void UpdateWorkspace(Callback callback = nullptr);
		/// @brief Capability::Submit: these files (the Renamed ones with their _originalPath) with message, then pushed if push
		/// (Capability::Push)
		void Submit(Vector<Path> paths, String message, bool push, Callback callback = nullptr);
		/// @brief Capability::Push
		void Push(Callback callback = nullptr);
		/// @brief Of the files unchanged among the paths (folders: everything under them), checked out by hand or by the editor
		/// ("Version Control/Revert Unchanged", what an editor crash left checked out...)
		void RevertUnchanged(Vector<Path> paths, Callback callback = nullptr);
		void OpenDiff(const Path& path);
		void OpenHistory(const Path& path);

		// Edition of an asset in a tab (by EditorTab: the tabs have nothing to do)

		/// @brief On the first change of a clean tab: checks its files out right away (Capability::Checkout, "checkoutOnEdit"
		/// user setting, on by default), not on save, so that a lock taken meanwhile is known after one change only (undo is
		/// enough). Those it checks out are remembered as the editor's, not the user's. Already checked out: nothing to do
		void PrepareEdit(Vector<Path> paths, Callback callback = nullptr);
		/// @brief On a tab closed: reverts those of the paths PrepareEdit checked out, if unchanged (RevertUnchanged). Never the
		/// ones checked out by hand (Checkout), nor those checked out before PrepareEdit
		void ReleaseEdit(Vector<Path> paths);

		// Synchronous: by AssetDatabase and the code writing the files of the project, from any thread: the calling one waits for the
		// worker when the provider has something to do (rarely: once per file to check out, never for the untracked ones)

		/// @brief Before writing files (a tab's save, a reimport, a thumbnail, the project settings): checks them out if the provider
		/// needs it (Capability::Checkout) and they aren't yet. Waits for the operations queued before. Immediate otherwise (no
		/// provider, new files)
		/// @return false if they can't be written (locked by someone else, checkout failed): the caller doesn't write, and reports error
		bool PrepareWrite(ArrayView<Path> paths, String* error = nullptr);

		/// @brief By AssetDatabase::Move/Delete: through the provider when it tracks moves and the files are tracked. NotHandled: the
		/// caller moves/deletes on disk as usual
		Delegation Move(const Path& from, const Path& to, String* error = nullptr);
		Delegation Delete(ArrayView<Path> paths, String* error = nullptr);

		/// @brief By AssetDatabase on new files of the project (created by the editor or found on disk): added if the user chose to
		/// ("Auto add" user setting, off by default)
		void OnFilesCreated(ArrayView<Path> paths);
		void SetAutoAdd(bool autoAdd);
		bool GetAutoAdd() const;
		void SetCheckoutOnEdit(bool checkoutOnEdit);
		bool GetCheckoutOnEdit() const;

	private:
		using Result = VersionControlProvider::Result;

		using Execute = std::function<Result(VersionControlProvider& provider, const Vector<Path>& paths)>;

		struct SyncState
		{
			Result _result;
			bool   _done = false; // _queueLock
		};

		struct Operation
		{
			String                     _name; // In the TaskTracker and the errors
			Vector<Path>               _paths;
			Execute                    _execute;
			Callback                   _callback;
			bool                       _refreshPaths = true; // The statuses of _paths, once done
			std::shared_ptr<SyncState> _sync;                // Synchronous (see RunSynchronous): no callback, no task
		};

		struct Completion
		{
			String   _name;
			Result   _result;
			Callback _callback;
		};

		void Connect(); // Main thread: chooses the provider, the worker connects it
		void Disconnect();
		void StartWorker();
		void StopWorker(); // Runs what's queued first

		void   Enqueue(Operation operation);
		void   EnqueueAsync(std::string_view name, Vector<Path> paths, Execute execute, Callback callback, bool refreshPaths = true);
		Result RunSynchronous(std::string_view name, Vector<Path> paths, Execute execute);
		void   PostCompletion(std::string_view name, const Result& result, Callback callback);
		/// @brief Checks out those of the paths needing it (GetFilesToCheckout), on the worker. editCheckouts: remembered (PrepareEdit)
		Result CheckoutToWrite(VersionControlProvider& provider, const Vector<Path>& paths, bool editCheckouts);

		static int WorkerEntry(void* parameter);
		void       WorkerLoop();
		void       WorkerConnect();
		void       WorkerRefresh(bool fullRefresh, Vector<Path> paths);
		void       WorkerRun(Operation& operation);
		void       UpdateCapabilities(); // Worker, once connected

		void ApplyStatuses(VersionControlProvider::StatusMap&& statuses, bool fullRefresh, ArrayView<Path> refreshedPaths);
		void RebuildFolderStatuses(); // _statusLock held

		/// @brief Without UserSettings.json and the paths out of the root folders
		Vector<Path> FilterPaths(ArrayView<Path> paths) const;
		bool         IsUnderRootFolders(const Path& path) const;
		bool         IsExcluded(const Path& path) const;
		/// @brief Of paths, the files on disk needing a checkout before being written (tracked, unchanged, not locked by me), from
		/// the cache. The ones locked by someone else are left out, listed in lockedByOthers
		Vector<Path>         GetFilesToCheckout(ArrayView<Path> paths, String& lockedByOthers) const;
		VersionControlStatus GetCachedStatus(const Path& path) const; // _statusLock held

		VersionControlProvider::WorkspaceSetup MakeWorkspaceSetup() const;

		void LoadUserSettings();
		void SaveUserSettings() const;

	private:
		Vector<VersionControlProvider*> _providers; // Owned by the EditorModuleRegistration of their module
		VersionControlProvider*         _provider = nullptr;
		std::atomic<bool>               _connected = false;
		std::atomic<uint32_t>           _capabilities = 0;
		std::atomic<uint32_t>           _pendingPushCount = 0;
		mutable std::mutex              _connectionErrorLock;
		String                          _connectionError;
		String                          _capabilityHints[VersionControlProvider::Capability::Count]; // _connectionErrorLock

		// User settings (UserSettings.json)
		String _providerChoice = "Auto";
		bool   _autoAdd = false;
		bool   _checkoutOnEdit = true;

		// Of the opened project, set on the main thread before the worker starts
		bool                                   _projectOpened = false;
		Path                                   _projectDir;
		Path                                   _userSettingsPath;
		Vector<Path>                           _projectFolders; // The asset, source, project settings and code folders, the project file
		Vector<Path>                           _rootFolders;    // _projectFolders and the provider's workspace files
		VersionControlProvider::WorkspaceSetup _workspaceSetup;

		// Checked out by PrepareEdit, released by ReleaseEdit. A Checkout by hand of one of them takes it out: the user's then.
		// In memory only: after a crash, "Revert Unchanged" by hand
		std::mutex               _editCheckoutsLock;
		std::unordered_set<Path> _editCheckouts;

		mutable std::mutex                             _statusLock;
		bool                                           _hasStatuses = false; // A full refresh was made
		std::unordered_map<Path, VersionControlStatus> _statuses;            // Unchanged files (without flags) left out
		std::unordered_map<Path, VersionControlStatus> _folderStatuses;
		std::atomic<uint32_t>                          _statusVersion = 0;
		std::atomic<uint32_t>                          _outOfDateCount = 0;

		using Clock = std::chrono::steady_clock;

		mutable std::mutex      _queueLock;
		std::condition_variable _queueCondition;
		std::condition_variable _syncCondition;
		Vector<Operation>       _queue;
		bool                    _connectRequested = false;
		bool                    _pendingFullRefresh = false;
		Vector<Path>            _pendingRefreshPaths; // Debounced (see RequestRefresh)
		Clock::time_point       _pendingRefreshTime;
		bool                    _stopWorker = false;
		String                  _lastRefreshError; // Worker: warned about once until it changes
		bool                    _workerRunning = false;
		std::atomic<bool>       _busy = false;
		Thread                  _worker;
		Vector<Completion>      _completions; // _queueLock, run by Update

		bool _editorFocused = true;
	};
}

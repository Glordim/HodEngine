#pragma once
#include "HodEngine/VersionControlEditor/Export.hpp"

#include <HodEngine/Editor/VersionControl/VersionControlProvider.hpp>

#include <HodEngine/Core/Process/Process.hpp>

#include <chrono>
#include <unordered_map>
#include <unordered_set>

namespace hod::inline editor
{
	/// @brief Git through its command line (git 2.25+), Git LFS (git-lfs) for the locks.
	///
	/// Every command runs in the root of the working tree, with root relative paths ("git paths", '/' separated, literal: no
	/// glob). The ones taking many paths read them from stdin (--pathspec-from-file), the others get them in chunks.
	///
	/// What comes from the server is refreshed on full refreshes only (QueryStatus), at most once a minute: a fetch (the out
	/// of date files: changed on the upstream branch since the merge base) and the LFS lock list (git lfs locks --verify).
	/// Lock and Checkout fetch first, a lock on a file changed upstream being refused.
	///
	/// Checkout: the files "lockable" in .gitattributes are read-only until locked (git lfs), Checkout locks them. Without a
	/// remote (no LFS server to hold the locks: a repository not published yet), Checkout makes them writable only.
	class HOD_VERSION_CONTROL_EDITOR_API GitProvider : public VersionControlProvider
	{
	public:
		GitProvider() = default;
		~GitProvider() override = default;

	public:
		std::string_view GetName() const override;

		bool   Detect(const Path& projectDir) const override;
		Result Connect(const Path& projectDir) override;
		void   Disconnect() override;
		Result CreateWorkspace(const Path& projectDir) override;
		bool   CanCreateWorkspace() const override;
		Result SetupWorkspace(const Path& projectDir, const WorkspaceSetup& setup) override;

		uint32_t GetCapabilities() const override;

		Result QueryStatus(ArrayView<Path> folders, StatusMap& statuses) override;
		Result QueryFileStatus(ArrayView<Path> paths, StatusMap& statuses) override;

		Result Add(ArrayView<Path> paths) override;
		Result Revert(ArrayView<Path> paths) override;
		Result RevertUnchanged(ArrayView<Path> paths) override;
		Result Checkout(ArrayView<Path> paths) override;
		Result Lock(ArrayView<Path> paths) override;
		Result Unlock(ArrayView<Path> paths) override;
		/// @brief Fetch, then a fast-forward merge of the upstream branch (the whole working tree: git can't do a part of it). Refused
		/// by git if the branch has commits of its own (to merge or rebase out of the editor) or if a local change would be overwritten
		Result UpdateWorkspace(ArrayView<Path> folders) override;
		/// @brief A local commit of these files only (git commit --only): what else is staged stays staged
		Result   Submit(ArrayView<Path> paths, std::string_view message) override;
		Result   Push() override;
		uint32_t GetPendingPushCount() const override;

		String       GetCapabilityHint(uint32_t capability) const override;
		Vector<Path> GetWorkspaceFiles(const Path& projectDir) const override;

		Result OpenDiff(const Path& path) override;
		Result OpenHistory(const Path& path) override;

		/// @brief Once connected: the root of the working tree, as git gives it
		const Path& GetRoot() const;
		/// @brief Once connected: git-lfs is installed
		bool HasLfs() const;

		/// @brief Delay between two fetches (and lock list refreshes) of QueryStatus, 60s by default. 0: each time
		void SetRemoteRefreshDelay(std::chrono::milliseconds delay);

	private:
		struct RunOptions
		{
			std::string_view _input;                      // Written to stdin (--pathspec-from-file=-)
			uint32_t         _timeoutMs = 2 * 60 * 1000;  // The network (git lfs lock) included
			const Path*      _workingDirectory = nullptr; // The root by default
			bool             _literalPathspecs = true;    // GIT_LITERAL_PATHSPECS
		};

		struct LockInfo
		{
			bool   _ours = false;
			String _owner;
		};

		Result Run(const Vector<String>& arguments, Process::Result& output, const RunOptions& options) const;
		Result Run(const Vector<String>& arguments, Process::Result& output) const;
		/// @brief arguments, then "--" and the paths, in as many runs as needed for the command line length. Outputs concatenated
		Result RunChunked(const Vector<String>& arguments, const Vector<String>& gitPaths, String& output, const RunOptions& options) const;
		Result RunChunked(const Vector<String>& arguments, const Vector<String>& gitPaths, String& output) const;
		/// @brief arguments + --pathspec-from-file=- --pathspec-file-nul, the paths on stdin
		Result RunWithPathspecs(Vector<String> arguments, const Vector<String>& gitPaths) const;

		/// @brief Empty if out of the project folder
		String ToGitPath(const Path& path) const;
		Path   FromGitPath(std::string_view gitPath) const;
		/// @brief Of the paths in the project folder
		Vector<String> ToGitPaths(ArrayView<Path> paths) const;
		static bool    IsSameOrUnder(std::string_view gitPath, std::string_view parent);
		static bool    IsUnderAny(std::string_view gitPath, const Vector<String>& parents);
		static void    SplitNul(std::string_view output, Vector<String>& entries);

		Result QueryLocalStatus(const Vector<String>& gitPaths, StatusMap& statuses) const;
		/// @brief _outOfDate and the locks, from what was last fetched
		void ApplyRemoteInfo(const Vector<String>& gitPaths, StatusMap& statuses) const;

		void   RefreshUpstream();
		Result Fetch();
		Result RefreshUpstreamChanges();
		Result RefreshLocks();
		void   RefreshRemoteInfo(bool force);
		void   RefreshPendingPushCount();

		/// @brief The tracked files under gitPaths
		Result ListTrackedFiles(const Vector<String>& gitPaths, Vector<String>& files) const;
		Result FilterLockable(const Vector<String>& files, Vector<String>& lockable) const;
		/// @brief Of the gitPaths (files), the ones changed locally or in commits not pushed yet
		Result ListUnreleasable(const Vector<String>& files, std::unordered_set<String>& changed) const;
		Result LockFiles(const Vector<String>& files);
		Result UnlockFiles(const Vector<String>& files);
		Result ReleaseUnchangedLocks(const Vector<String>& gitPaths);
		/// @brief The read-only flag git-lfs puts on the lockable files removed, without a lock (no remote to lock them on)
		Result MakeWritable(const Vector<String>& files);

		/// @brief The .gitattributes / .gitignore of the project folder and of its parents in the working tree (and .git/info)
		bool FindUp(std::string_view fileName) const;
		void UpdateAttributes();

	private:
		Path   _projectDir;
		Path   _root;
		String _prefix; // Of the project folder in the working tree ("Games/MyGame/", "" at the root)
		bool   _connected = false;

		bool _hasLfs = false;      // git-lfs installed
		bool _usesLfs = false;     // filter=lfs in the attributes
		bool _hasLockable = false; // lockable in the attributes
		bool _hasUpstream = false;
		bool _hasRemote = false; // A remote (or lfs.url): where the LFS locks are. None: no lock, no network

		uint32_t _pendingPushCount = 0; // Commits of HEAD not on its upstream

		std::unordered_map<String, LockInfo> _locks;           // By git path, as of the last RefreshLocks (plus ours since)
		std::unordered_set<String>           _upstreamChanges; // Git paths changed on the upstream since the merge base

		std::chrono::milliseconds             _remoteRefreshDelay = std::chrono::seconds(60);
		std::chrono::steady_clock::time_point _lastRemoteRefresh;
		bool                                  _remoteRefreshed = false;
		bool                                  _fetchWarned = false;
		bool                                  _locksWarned = false;
		bool                                  _noRemoteWarned = false;
	};
}

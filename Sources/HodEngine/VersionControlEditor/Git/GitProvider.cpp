#include "HodEngine/VersionControlEditor/Pch.hpp"
#include "HodEngine/VersionControlEditor/Git/GitProvider.hpp"

#include <HodEngine/Core/Document/Document.hpp>
#include <HodEngine/Core/Document/DocumentReaderJson.hpp>
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/UID.hpp>

#include <cstdlib>
#include <filesystem>

#include <fmt/format.h>

namespace hod::inline editor
{
	namespace
	{
		/// @brief Of the paths given to a command line, well under the 32k characters of Windows
		constexpr uint32_t MaxChunkLength = 16 * 1024;

		/// @brief Without the spaces and line ends around
		std::string_view Trim(std::string_view text)
		{
			while (text.empty() == false && (text.front() == ' ' || text.front() == '\n' || text.front() == '\r' || text.front() == '\t'))
			{
				text.remove_prefix(1);
			}
			while (text.empty() == false && (text.back() == ' ' || text.back() == '\n' || text.back() == '\r' || text.back() == '\t'))
			{
				text.remove_suffix(1);
			}
			return text;
		}

		/// @brief For --pathspec-file-nul and check-attr -z --stdin
		String JoinNul(const Vector<String>& entries)
		{
			String joined;
			for (const String& entry : entries)
			{
				joined += entry;
				joined += '\0';
			}
			return joined;
		}

		Vector<String> SplitLines(std::string_view text)
		{
			Vector<String> lines;
			while (text.empty() == false)
			{
				const size_t     end = text.find('\n');
				std::string_view line = text.substr(0, end);
				if (line.empty() == false && line.back() == '\r')
				{
					line.remove_suffix(1);
				}
				lines.PushBack(String(line));
				text = end == std::string_view::npos ? std::string_view() : text.substr(end + 1);
			}
			return lines;
		}

		/// @brief Of a porcelain v2 entry: what follows its fieldCount first fields (the path may have spaces)
		std::string_view SkipFields(std::string_view entry, uint32_t fieldCount)
		{
			for (uint32_t field = 0; field < fieldCount; ++field)
			{
				const size_t space = entry.find(' ');
				if (space == std::string_view::npos)
				{
					return std::string_view();
				}
				entry.remove_prefix(space + 1);
			}
			return entry;
		}

		String JoinErrors(const Vector<String>& errors)
		{
			String joined;
			for (const String& error : errors)
			{
				if (joined.Empty() == false)
				{
					joined += '\n';
				}
				joined += error;
			}
			return joined;
		}
	}

	/// @brief
	/// @return
	std::string_view GitProvider::GetName() const
	{
		return "Git";
	}

	/// @brief A .git (folder, or file of a worktree/submodule) in the project folder or up its parents
	/// @param projectDir
	/// @return
	bool GitProvider::Detect(const Path& projectDir) const
	{
		Path directory = projectDir;
		while (directory.Empty() == false)
		{
			if (FileSystem::GetInstance()->Exists(directory / ".git"))
			{
				return true;
			}
			Path parent = directory.ParentPath();
			if (parent == directory)
			{
				break;
			}
			directory = parent;
		}
		return false;
	}

	/// @brief
	/// @param projectDir
	/// @return
	VersionControlProvider::Result GitProvider::Connect(const Path& projectDir)
	{
		Disconnect();
		_projectDir = projectDir;

		Process::Result output;
		RunOptions      options;
		options._workingDirectory = &_projectDir;
		Result result = Run({"rev-parse", "--show-toplevel", "--show-prefix"}, output, options);
		if (result._success == false)
		{
			return result;
		}
		Vector<String> lines = SplitLines(output._output);
		if (lines.Empty() || lines[0].Empty())
		{
			return Result::Failure(fmt::format("git rev-parse: unexpected output \"{}\"", Trim(output._output)));
		}
		_root = Path(lines[0]);
		_prefix = lines.Size() > 1 ? lines[1] : String();

		_hasLfs = Run({"lfs", "version"}, output)._success;
		UpdateAttributes();
		if (_usesLfs && _hasLfs == false)
		{
			OUTPUT_WARNING("Git: this repository stores files with Git LFS but git-lfs isn't installed: these files are pointers, not their content. Install "
			               "it (https://git-lfs.com), then run \"git lfs install\" and \"git lfs pull\"");
		}
		RefreshUpstream();

		_connected = true;
		return Result();
	}

	/// @brief
	void GitProvider::Disconnect()
	{
		_connected = false;
		_projectDir.Clear();
		_root.Clear();
		_prefix.Clear();
		_hasLfs = false;
		_usesLfs = false;
		_hasLockable = false;
		_hasUpstream = false;
		_hasRemote = false;
		_locks.clear();
		_upstreamChanges.clear();
		_remoteRefreshed = false;
		_fetchWarned = false;
		_locksWarned = false;
		_noRemoteWarned = false;
	}

	/// @brief git init in the project folder (the .gitignore and .gitattributes come with SetupWorkspace, after Connect)
	/// @param projectDir
	/// @return
	VersionControlProvider::Result GitProvider::CreateWorkspace(const Path& projectDir)
	{
		Process::Result output;
		RunOptions      options;
		options._workingDirectory = &projectDir;
		return Run({"init", "--quiet"}, output, options);
	}

	/// @brief
	/// @return
	bool GitProvider::CanCreateWorkspace() const
	{
		return true;
	}

	/// @brief .gitignore and .gitattributes, each only if none exists from the project folder up to the root
	/// @param projectDir
	/// @param setup
	/// @return
	VersionControlProvider::Result GitProvider::SetupWorkspace(const Path& projectDir, const WorkspaceSetup& setup)
	{
		(void)projectDir; // The one given to Connect

		Vector<String> created;
		Vector<String> errors;

		// Relative to the project folder, '/' separated, anchored ("/Cache/": only this one)
		auto toPattern = [this](const Path& path)
		{
			String pattern = ToGitPath(path);
			if (pattern.StartsWith(_prefix))
			{
				pattern = pattern.SubStr(_prefix.Size());
			}
			return String("/") + pattern;
		};

		if (FindUp(".gitignore") == false)
		{
			String content = "# Created by the HodEngine editor: what it generates, and the settings of each user\n";
			for (const Path& folder : setup._ignoredFolders)
			{
				content += toPattern(folder) + "/\n";
			}
			for (const Path& file : setup._ignoredFiles)
			{
				content += toPattern(file) + "\n";
			}
			if (FileSystem::GetInstance()->WriteAllText(_projectDir / ".gitignore", content))
			{
				created.PushBack(".gitignore");
			}
			else
			{
				errors.PushBack(fmt::format("Can't write {}", _projectDir / ".gitignore").c_str());
			}
		}

		if (FindUp(".gitattributes") == false && setup._binaryExtensions.Empty() == false)
		{
			// LFS only for files not committed yet: in an existing history, it would change how the files already there are stored
			// (all shown modified). git lfs migrate does that, rewriting the history: the team's decision
			bool useLfs = _hasLfs;
			if (useLfs)
			{
				Vector<String> patterns;
				for (const String& extension : setup._binaryExtensions)
				{
					patterns.PushBack(String("*.") + extension);
				}
				String     tracked;
				RunOptions options;
				options._literalPathspecs = false;
				if (RunChunked({"ls-files", "-z", "--full-name"}, patterns, tracked, options)._success == false || tracked.Empty() == false)
				{
					useLfs = false;
				}
			}

			String content = useLfs ? "# Created by the HodEngine editor: the assets and their binary sources aren't mergeable, stored with Git LFS and locked to be edited\n"
			                        : "# Created by the HodEngine editor: the assets and their binary sources aren't mergeable\n";
			String extensionList;
			for (const String& extension : setup._binaryExtensions)
			{
				content += String("*.") + extension + (useLfs ? " filter=lfs diff=lfs merge=lfs -text lockable\n" : " binary\n");
				extensionList += extensionList.Empty() ? String("*.") + extension : String(",*.") + extension;
			}
			if (FileSystem::GetInstance()->WriteAllText(_projectDir / ".gitattributes", content))
			{
				created.PushBack(".gitattributes");

				Process::Result output;
				if (useLfs && Run({"config", "--get", "filter.lfs.process"}, output)._success == false)
				{
					// The filters and the hooks (the lockable files read-only until locked) of git-lfs, for this repository
					Result installed = Run({"lfs", "install", "--local"}, output);
					if (installed._success == false)
					{
						errors.PushBack(installed._error);
					}
				}

				if (_hasLfs == false)
				{
					OUTPUT_WARNING("Git: git-lfs isn't installed (https://git-lfs.com): the created .gitattributes marks the assets binary only, they can't be "
					               "locked and the repository will grow with each of their versions");
				}
				else if (useLfs == false)
				{
					OUTPUT_WARNING("Git: assets or binary sources are committed already, without Git LFS: the created .gitattributes marks them binary only (no "
					               "LFS, no lock). To move them to Git LFS, which rewrites the history: git lfs migrate import --everything --include=\"{}\"",
					               extensionList);
				}
			}
			else
			{
				errors.PushBack(fmt::format("Can't write {}", _projectDir / ".gitattributes").c_str());
			}
			UpdateAttributes();
		}

		if (created.Empty() == false)
		{
			String names;
			for (const String& name : created)
			{
				names += names.Empty() ? name : String(" and ") + name;
			}
			OUTPUT_MESSAGE("Git: created {} in {}, to commit", names, _projectDir);
		}

		return errors.Empty() ? Result() : Result::Failure(JoinErrors(errors));
	}

	/// @brief
	/// @return
	uint32_t GitProvider::GetCapabilities() const
	{
		uint32_t capabilities = Capability::Diff | Capability::History | Capability::Submit;
		if (_hasUpstream)
		{
			capabilities |= Capability::RemoteStatus | Capability::UpdateWorkspace | Capability::Push;
		}
		if (_hasLfs && _usesLfs)
		{
			if (_hasRemote)
			{
				capabilities |= Capability::Lock;
			}
			if (_hasLockable) // Even without a remote: the lockable files are read-only, Checkout makes them writable
			{
				capabilities |= Capability::Checkout;
			}
		}
		return capabilities;
	}

	/// @brief
	/// @param folders
	/// @param statuses
	/// @return
	VersionControlProvider::Result GitProvider::QueryStatus(ArrayView<Path> folders, StatusMap& statuses)
	{
		const Vector<String> gitPaths = ToGitPaths(folders);
		RefreshRemoteInfo(false);
		RefreshPendingPushCount();
		Result result = QueryLocalStatus(gitPaths, statuses);
		ApplyRemoteInfo(gitPaths, statuses);
		return result;
	}

	/// @brief
	/// @param paths
	/// @param statuses
	/// @return
	VersionControlProvider::Result GitProvider::QueryFileStatus(ArrayView<Path> paths, StatusMap& statuses)
	{
		const Vector<String> gitPaths = ToGitPaths(paths);
		Result               result = QueryLocalStatus(gitPaths, statuses);
		ApplyRemoteInfo(gitPaths, statuses);
		return result;
	}

	/// @brief Staged, those not on disk left out (git add refuses them)
	/// @param paths
	/// @return
	VersionControlProvider::Result GitProvider::Add(ArrayView<Path> paths)
	{
		Vector<String> gitPaths;
		for (const Path& path : paths)
		{
			if (FileSystem::GetInstance()->Exists(path))
			{
				String gitPath = ToGitPath(path);
				if (gitPath.Empty() == false)
				{
					gitPaths.PushBack(std::move(gitPath));
				}
			}
		}
		if (gitPaths.Empty())
		{
			return Result();
		}
		return RunWithPathspecs({"add"}, gitPaths);
	}

	/// @brief Unstaged (the added ones untracked again, left on disk), the ones of HEAD restored, then their locks released
	/// @param paths
	/// @return
	VersionControlProvider::Result GitProvider::Revert(ArrayView<Path> paths)
	{
		const Vector<String> gitPaths = ToGitPaths(paths);
		if (gitPaths.Empty())
		{
			return Result();
		}

		Process::Result output;
		if (Run({"rev-parse", "--verify", "--quiet", "HEAD"}, output)._success == false)
		{
			// No commit yet: nothing to restore, only to unstage
			return RunWithPathspecs({"rm", "--cached", "-r", "-q", "--ignore-unmatch"}, gitPaths);
		}

		String inHeadOutput;
		Result result = RunChunked({"ls-tree", "-r", "-z", "--name-only", "--full-name", "HEAD"}, gitPaths, inHeadOutput);
		if (result._success == false)
		{
			return result;
		}
		Vector<String> inHead;
		SplitNul(inHeadOutput, inHead);

		result = RunWithPathspecs({"reset", "-q"}, gitPaths);
		if (result._success == false)
		{
			return result;
		}
		if (inHead.Empty() == false)
		{
			result = RunWithPathspecs({"restore", "--worktree", "--source=HEAD"}, inHead);
			if (result._success == false)
			{
				return result;
			}
		}
		return ReleaseUnchangedLocks(gitPaths);
	}

	/// @brief Our locks on the unchanged files (not modified, not in a commit not pushed yet)
	/// @param paths
	/// @return
	VersionControlProvider::Result GitProvider::RevertUnchanged(ArrayView<Path> paths)
	{
		return ReleaseUnchangedLocks(ToGitPaths(paths));
	}

	/// @brief Locks the lockable files (read-only until locked) not locked by us yet
	/// @param paths
	/// @return
	VersionControlProvider::Result GitProvider::Checkout(ArrayView<Path> paths)
	{
		if (_hasLfs == false || _hasLockable == false)
		{
			return Result();
		}

		Vector<String> files;
		Result         result = ListTrackedFiles(ToGitPaths(paths), files);
		if (result._success == false)
		{
			return result;
		}
		Vector<String> lockable;
		result = FilterLockable(files, lockable);
		if (result._success == false)
		{
			return result;
		}

		if (_hasRemote == false)
		{
			if (lockable.Empty() == false && _noRemoteWarned == false)
			{
				OUTPUT_WARNING("Git: no remote, the lockable files can't be locked (the LFS locks are on the server): they are made writable without a lock");
				_noRemoteWarned = true;
			}
			return MakeWritable(lockable);
		}

		Vector<String> toLock;
		for (const String& file : lockable)
		{
			auto it = _locks.find(file);
			if (it == _locks.end() || it->second._ours == false)
			{
				toLock.PushBack(file);
			}
		}
		return LockFiles(toLock);
	}

	/// @brief
	/// @param paths
	/// @return
	VersionControlProvider::Result GitProvider::Lock(ArrayView<Path> paths)
	{
		if (_hasLfs == false || _usesLfs == false)
		{
			return NotSupported("lock without Git LFS");
		}

		Vector<String> files;
		Result         result = ListTrackedFiles(ToGitPaths(paths), files);
		if (result._success == false)
		{
			return result;
		}
		if (files.Empty())
		{
			return Result::Failure("Git: only committed files can be locked");
		}
		return LockFiles(files);
	}

	/// @brief Ours only (someone else's needs --force: not from the editor)
	/// @param paths
	/// @return
	VersionControlProvider::Result GitProvider::Unlock(ArrayView<Path> paths)
	{
		if (_hasLfs == false || _usesLfs == false)
		{
			return NotSupported("unlock without Git LFS");
		}

		const Vector<String> gitPaths = ToGitPaths(paths);
		Vector<String>       files;
		for (const auto& [file, lock] : _locks)
		{
			if (lock._ours && IsUnderAny(file, gitPaths))
			{
				files.PushBack(file);
			}
		}
		return UnlockFiles(files);
	}

	/// @brief
	/// @param folders
	/// @return
	VersionControlProvider::Result GitProvider::UpdateWorkspace(ArrayView<Path> folders)
	{
		(void)folders;
		RefreshUpstream();
		if (_hasUpstream == false)
		{
			return Result::Failure("Git: the current branch has no upstream branch to pull from (git branch --set-upstream-to)");
		}

		Result result = Fetch();
		if (result._success == false)
		{
			return result;
		}
		_lastRemoteRefresh = std::chrono::steady_clock::now();
		_remoteRefreshed = true;

		Process::Result output;
		RunOptions      options;
		options._timeoutMs = 30 * 60 * 1000; // The LFS files are downloaded by the checkout
		result = Run({"merge", "--ff-only", "--no-edit", "--quiet", "@{u}"}, output, options);
		RefreshUpstreamChanges(); // What's left out of date: all of it if refused
		RefreshPendingPushCount();
		if (result._success == false && result._error.Find("fast-forward") != String::Npos)
		{
			result._error = "Git: can't fast-forward, the branch has commits not pushed yet: merge or rebase them out of the editor";
		}
		return result;
	}

	/// @brief The ones on disk added (untracked included), the others removed from the index, then committed with these paths only.
	/// The message through a file: any character, any length
	/// @param paths
	/// @param message
	/// @return
	VersionControlProvider::Result GitProvider::Submit(ArrayView<Path> paths, std::string_view message)
	{
		Vector<String> onDisk;
		Vector<String> removed;
		for (const Path& path : paths)
		{
			String gitPath = ToGitPath(path);
			if (gitPath.Empty() == false)
			{
				(FileSystem::GetInstance()->Exists(path) ? onDisk : removed).PushBack(std::move(gitPath));
			}
		}
		if (onDisk.Empty() && removed.Empty())
		{
			return Result::Failure("Git: nothing to commit");
		}

		Result result = RunWithPathspecs({"add", "-A"}, onDisk);
		if (result._success == false)
		{
			return result;
		}
		result = RunWithPathspecs({"rm", "--cached", "-r", "-q", "--ignore-unmatch"}, removed);
		if (result._success == false)
		{
			return result;
		}

		const Path messageFile = Path::Temp() / (String("HodCommitMessage_") + UID::GenerateUID().ToString());
		if (FileSystem::GetInstance()->WriteAllText(messageFile, String(message)) == false)
		{
			return Result::Failure(fmt::format("Git: can't write the commit message to {}", messageFile));
		}
		Vector<String> all = std::move(onDisk);
		for (String& gitPath : removed)
		{
			all.PushBack(std::move(gitPath));
		}
		result = RunWithPathspecs({"commit", "-q", "--cleanup=strip", "-F", messageFile.GetString()}, all);
		FileSystem::GetInstance()->RemoveAll(messageFile);
		RefreshPendingPushCount();
		return result;
	}

	/// @brief To the upstream branch, then our locks of the files of the project no longer changed nor in a commit not pushed released
	/// @return
	VersionControlProvider::Result GitProvider::Push()
	{
		RefreshUpstream();
		if (_hasUpstream == false)
		{
			return Result::Failure("Git: the current branch has no upstream branch (push it once with git push -u origin <branch>)");
		}

		Process::Result output;
		RunOptions      options;
		options._timeoutMs = 30 * 60 * 1000; // The LFS files are uploaded
		Result result = Run({"push", "--quiet"}, output, options);
		if (result._success == false)
		{
			if (result._error.Find("[rejected]") != String::Npos || result._error.Find("fetch first") != String::Npos)
			{
				result._error = "Git: the server has commits you don't have yet: Pull first (merge or rebase out of the editor if the branch diverged)";
			}
			RefreshPendingPushCount();
			return result;
		}

		RefreshPendingPushCount();
		RefreshUpstreamChanges();
		return ReleaseUnchangedLocks({_prefix.Empty() ? String(".") : _prefix.SubStr(0, _prefix.Size() - 1)});
	}

	/// @brief
	/// @return
	uint32_t GitProvider::GetPendingPushCount() const
	{
		return _pendingPushCount;
	}

	/// @brief
	/// @param capability
	/// @return
	String GitProvider::GetCapabilityHint(uint32_t capability) const
	{
		switch (capability)
		{
			case Capability::Lock:
				if (_hasLfs == false)
				{
					return "Git: locks need Git LFS (git-lfs isn't installed)";
				}
				return _usesLfs ? "Git: the LFS locks are on the server, this repository has no remote (git remote add origin <url>)"
				                : "Git: locks need the binary files in Git LFS (filter=lfs in .gitattributes)";
			case Capability::Checkout: return "Git: a checkout locks the files marked \"lockable\" in .gitattributes (Git LFS), there are none";
			case Capability::GetLatestFiles: return "Git can't update some files only: Pull updates the whole project";
			case Capability::TrackMoves: return "Git detects the moves on its own";
			case Capability::RemoteStatus:
			case Capability::UpdateWorkspace:
			case Capability::Push: return "Git: the current branch has no upstream branch (git push -u origin <branch> once)";
			default: break;
		}
		return VersionControlProvider::GetCapabilityHint(capability);
	}

	/// @brief The ones SetupWorkspace creates
	/// @param projectDir
	/// @return
	Vector<Path> GitProvider::GetWorkspaceFiles(const Path& projectDir) const
	{
		return {projectDir / ".gitignore", projectDir / ".gitattributes"};
	}

	/// @brief git difftool, only if a diff (or merge) tool is configured: git would fall back on a terminal tool otherwise, invisible
	/// @param path
	/// @return
	VersionControlProvider::Result GitProvider::OpenDiff(const Path& path)
	{
		const String gitPath = ToGitPath(path);
		if (gitPath.Empty())
		{
			return Result::Failure(fmt::format("{} isn't in the project", path));
		}

		Process::Result output;
		if (Run({"config", "--get", "diff.tool"}, output)._success == false && Run({"config", "--get", "merge.tool"}, output)._success == false)
		{
			return Result::Failure("Git: no diff tool configured (git config --global diff.tool <tool>, see git difftool --tool-help)");
		}

		Process::Desc desc;
		desc._program = "git";
		desc._arguments = {"difftool", "--no-prompt", "--", gitPath};
		desc._workingDirectory = _root;
		desc._environment.PushBack({"GIT_TERMINAL_PROMPT", "0"});
		desc._environment.PushBack({"GIT_LITERAL_PATHSPECS", "1"});
		if (Process::Launch(desc) == false)
		{
			return Result::Failure("Git: can't start git difftool");
		}
		return Result();
	}

	/// @brief gitk (installed with Git for Windows, a package of its own on Linux)
	/// @param path
	/// @return
	VersionControlProvider::Result GitProvider::OpenHistory(const Path& path)
	{
		const String gitPath = ToGitPath(path);
		if (gitPath.Empty())
		{
			return Result::Failure(fmt::format("{} isn't in the project", path));
		}

		Process::Desc desc;
		desc._program = "gitk";
		desc._arguments = {"--", gitPath};
		desc._workingDirectory = _root;
		desc._environment.PushBack({"GIT_LITERAL_PATHSPECS", "1"});
		if (Process::Launch(desc) == false)
		{
			return Result::Failure("Git: can't start gitk (is it installed?)");
		}
		return Result();
	}

	/// @brief
	/// @return
	const Path& GitProvider::GetRoot() const
	{
		return _root;
	}

	/// @brief
	/// @return
	bool GitProvider::HasLfs() const
	{
		return _hasLfs;
	}

	/// @brief
	/// @param delay
	void GitProvider::SetRemoteRefreshDelay(std::chrono::milliseconds delay)
	{
		_remoteRefreshDelay = delay;
	}

	/// @brief Never interactive: no credential prompt (terminal or Git Credential Manager), a missing login is an error
	/// @param arguments
	/// @param output
	/// @param options
	/// @return
	VersionControlProvider::Result GitProvider::Run(const Vector<String>& arguments, Process::Result& output, const RunOptions& options) const
	{
		Process::Desc desc;
		desc._program = "git";
		desc._arguments = arguments;
		desc._workingDirectory = options._workingDirectory != nullptr ? *options._workingDirectory : _root;
		desc._environment.PushBack({"GIT_TERMINAL_PROMPT", "0"});
		desc._environment.PushBack({"GCM_INTERACTIVE", "never"});
		desc._environment.PushBack({"GIT_OPTIONAL_LOCKS", "0"}); // git status without the index lock: refreshes in the background
		desc._environment.PushBack({"GIT_LITERAL_PATHSPECS", options._literalPathspecs ? "1" : ""});
		desc._input = options._input;
		desc._timeoutMs = options._timeoutMs;

		output = Process::Result();
		const std::string_view command = arguments.Empty() ? std::string_view() : std::string_view(arguments[0]);
		if (Process::Run(desc, output) == false)
		{
			return Result::Failure(fmt::format("Can't run git, is it installed (and in the PATH)? {}", Trim(output._error)));
		}
		if (output._timedOut)
		{
			return Result::Failure(fmt::format("git {}: no answer after {}s", command, options._timeoutMs / 1000));
		}
		if (output._exitCode != 0)
		{
			std::string_view error = Trim(output._error);
			if (error.empty())
			{
				error = Trim(output._output);
			}
			return Result::Failure(fmt::format("git {}: {}", command, error));
		}
		return Result();
	}

	/// @brief
	/// @param arguments
	/// @param output
	/// @return
	VersionControlProvider::Result GitProvider::Run(const Vector<String>& arguments, Process::Result& output) const
	{
		return Run(arguments, output, RunOptions());
	}

	/// @brief
	/// @param arguments
	/// @param gitPaths
	/// @param output
	/// @param options
	/// @return
	VersionControlProvider::Result GitProvider::RunChunked(const Vector<String>& arguments, const Vector<String>& gitPaths, String& output, const RunOptions& options) const
	{
		output.Clear();
		uint32_t index = 0;
		while (index < gitPaths.Size())
		{
			Vector<String> chunk = arguments;
			chunk.PushBack("--");
			uint32_t length = 0;
			while (index < gitPaths.Size() && (length == 0 || length + gitPaths[index].Size() < MaxChunkLength))
			{
				length += gitPaths[index].Size() + 3; // Quotes and space
				chunk.PushBack(gitPaths[index]);
				++index;
			}

			Process::Result chunkOutput;
			Result          result = Run(chunk, chunkOutput, options);
			if (result._success == false)
			{
				return result;
			}
			output += chunkOutput._output;
		}
		return Result();
	}

	/// @brief
	/// @param arguments
	/// @param gitPaths
	/// @param output
	/// @return
	VersionControlProvider::Result GitProvider::RunChunked(const Vector<String>& arguments, const Vector<String>& gitPaths, String& output) const
	{
		return RunChunked(arguments, gitPaths, output, RunOptions());
	}

	/// @brief
	/// @param arguments
	/// @param gitPaths
	/// @return
	VersionControlProvider::Result GitProvider::RunWithPathspecs(Vector<String> arguments, const Vector<String>& gitPaths) const
	{
		if (gitPaths.Empty())
		{
			return Result();
		}
		const String input = JoinNul(gitPaths);
		arguments.PushBack("--pathspec-from-file=-");
		arguments.PushBack("--pathspec-file-nul");

		RunOptions options;
		options._input = input;
		Process::Result output;
		return Run(arguments, output, options);
	}

	/// @brief Relative to the root, '/' separated: the prefix of the project folder, then the path in it
	/// @param path
	/// @return
	String GitProvider::ToGitPath(const Path& path) const
	{
		const String& full = path.GetString();
		const String& base = _projectDir.GetString();
		if (base.Empty() || full.StartsWith(base) == false)
		{
			return String();
		}
		uint32_t position = base.Size();
		if (full.Size() > position && Path::IsSeparator(base.Back()) == false)
		{
			if (Path::IsSeparator(full[position]) == false)
			{
				return String(); // "Game2/..." isn't in "Game"
			}
			++position;
		}

		String gitPath = _prefix + full.SubStr(position);
		for (char& character : gitPath)
		{
			if (character == '\\')
			{
				character = '/';
			}
		}
		while (gitPath.Empty() == false && gitPath.Back() == '/')
		{
			gitPath.PopBack();
		}
		return gitPath.Empty() ? String(".") : gitPath;
	}

	/// @brief
	/// @param gitPath
	/// @return Empty if out of the project folder
	Path GitProvider::FromGitPath(std::string_view gitPath) const
	{
		if (gitPath.starts_with(std::string_view(_prefix)) == false)
		{
			return Path();
		}
		gitPath.remove_prefix(_prefix.Size());
		return gitPath.empty() ? _projectDir : _projectDir / Path(gitPath);
	}

	/// @brief
	/// @param paths
	/// @return
	Vector<String> GitProvider::ToGitPaths(ArrayView<Path> paths) const
	{
		Vector<String> gitPaths;
		gitPaths.Reserve(paths.Size());
		for (const Path& path : paths)
		{
			String gitPath = ToGitPath(path);
			if (gitPath.Empty() == false)
			{
				gitPaths.PushBack(std::move(gitPath));
			}
		}
		return gitPaths;
	}

	/// @brief
	/// @param gitPath
	/// @param parent
	/// @return
	bool GitProvider::IsSameOrUnder(std::string_view gitPath, std::string_view parent)
	{
		if (parent == ".")
		{
			return true;
		}
		return gitPath.starts_with(parent) && (gitPath.size() == parent.size() || gitPath[parent.size()] == '/');
	}

	/// @brief
	/// @param gitPath
	/// @param parents
	/// @return
	bool GitProvider::IsUnderAny(std::string_view gitPath, const Vector<String>& parents)
	{
		for (const String& parent : parents)
		{
			if (IsSameOrUnder(gitPath, parent))
			{
				return true;
			}
		}
		return false;
	}

	/// @brief The entries of a -z output (empty ones and line ends left out: git log -z separates the commits with them)
	/// @param output
	/// @param entries
	void GitProvider::SplitNul(std::string_view output, Vector<String>& entries)
	{
		while (output.empty() == false)
		{
			const size_t     end = output.find('\0');
			std::string_view entry = Trim(output.substr(0, end));
			if (entry.empty() == false)
			{
				entries.PushBack(String(entry));
			}
			output = end == std::string_view::npos ? std::string_view() : output.substr(end + 1);
		}
	}

	/// @brief git status --porcelain=v2 (see its documentation for the entries)
	/// @param gitPaths
	/// @param statuses
	/// @return
	VersionControlProvider::Result GitProvider::QueryLocalStatus(const Vector<String>& gitPaths, StatusMap& statuses) const
	{
		String output;
		Result result = RunChunked({"status", "--porcelain=v2", "-z", "--untracked-files=all", "--ignored=matching"}, gitPaths, output);
		if (result._success == false)
		{
			return result;
		}

		// Not SplitNul: a path may start or end with a space
		Vector<std::string_view> entries;
		std::string_view         remaining = output;
		while (remaining.empty() == false)
		{
			const size_t end = remaining.find('\0');
			entries.PushBack(remaining.substr(0, end));
			remaining = end == std::string_view::npos ? std::string_view() : remaining.substr(end + 1);
		}

		for (uint32_t index = 0; index < entries.Size(); ++index)
		{
			const std::string_view entry = entries[index];
			if (entry.size() < 3)
			{
				continue;
			}

			VersionControlStatus status;
			std::string_view     gitPath;
			const char           type = entry[0];
			if (type == '?' || type == '!')
			{
				status._state = type == '?' ? VersionControlState::Untracked : VersionControlState::Ignored;
				gitPath = entry.substr(2);
			}
			else if (type == '1' || type == '2' || type == 'u')
			{
				// <type> <XY> <sub> <mH> <mI> <mW> <hH> <hI> [<Xscore> (2)] / u <XY> <sub> <m1> <m2> <m3> <mW> <h1> <h2> <h3>
				const char staged = entry[2];
				const char worktree = entry[3];
				gitPath = SkipFields(entry, type == '1' ? 8 : (type == '2' ? 9 : 10));

				if (type == 'u')
				{
					status._state = VersionControlState::Conflicted;
				}
				else if (staged == 'D' || worktree == 'D')
				{
					status._state = VersionControlState::Deleted;
				}
				else if (type == '2')
				{
					status._state = VersionControlState::Renamed;
				}
				else if (staged == 'A')
				{
					status._state = VersionControlState::Added;
				}
				else
				{
					status._state = VersionControlState::Modified;
				}

				if (type == '2' && index + 1 < entries.Size())
				{
					++index; // The original path
					if (staged == 'R')
					{
						Path originalPath = FromGitPath(entries[index]);
						if (originalPath.Empty() == false)
						{
							statuses[originalPath]._state = VersionControlState::Deleted;
							status._originalPath = std::move(originalPath);
						}
					}
				}
			}
			else
			{
				continue;
			}

			Path path = FromGitPath(gitPath);
			if (path.Empty() == false)
			{
				statuses[path]._state = status._state;
				statuses[path]._originalPath = std::move(status._originalPath);
			}
		}
		return Result();
	}

	/// @brief
	/// @param gitPaths
	/// @param statuses
	void GitProvider::ApplyRemoteInfo(const Vector<String>& gitPaths, StatusMap& statuses) const
	{
		for (const String& changed : _upstreamChanges)
		{
			if (IsUnderAny(changed, gitPaths))
			{
				Path path = FromGitPath(changed);
				if (path.Empty() == false)
				{
					VersionControlStatus& status = statuses[path];
					if (status._state == VersionControlState::Unknown)
					{
						status._state = VersionControlState::Unchanged;
					}
					status._outOfDate = true;
				}
			}
		}

		for (const auto& [file, lock] : _locks)
		{
			if (IsUnderAny(file, gitPaths))
			{
				Path path = FromGitPath(file);
				if (path.Empty() == false)
				{
					VersionControlStatus& status = statuses[path];
					if (status._state == VersionControlState::Unknown)
					{
						status._state = VersionControlState::Unchanged;
					}
					status._lockedByMe = lock._ours;
					status._lockedByOther = lock._ours == false;
					status._otherUsers = lock._ours ? String() : lock._owner;
				}
			}
		}
	}

	/// @brief
	void GitProvider::RefreshPendingPushCount()
	{
		_pendingPushCount = 0;
		if (_hasUpstream == false)
		{
			return;
		}
		Process::Result output;
		if (Run({"rev-list", "--count", "@{u}..HEAD"}, output)._success)
		{
			_pendingPushCount = static_cast<uint32_t>(std::strtoul(output._output.CStr(), nullptr, 10));
		}
	}

	/// @brief Whether the current branch has an upstream (it can change: a checkout of another branch)
	void GitProvider::RefreshUpstream()
	{
		Process::Result output;
		_hasUpstream = Run({"rev-parse", "--abbrev-ref", "--symbolic-full-name", "@{u}"}, output)._success;
		_hasRemote = _hasUpstream || (Run({"remote"}, output)._success && Trim(output._output).empty() == false) || Run({"config", "--get", "lfs.url"}, output)._success;
	}

	/// @brief
	/// @return
	VersionControlProvider::Result GitProvider::Fetch()
	{
		Process::Result output;
		RunOptions      options;
		options._timeoutMs = 60 * 1000;
		return Run({"fetch", "--quiet", "--no-tags"}, output, options);
	}

	/// @brief What changed on the upstream since the merge base, in the project folder
	/// @return
	VersionControlProvider::Result GitProvider::RefreshUpstreamChanges()
	{
		_upstreamChanges.clear();
		if (_hasUpstream == false)
		{
			return Result();
		}

		String output;
		Result result = RunChunked({"diff", "--name-only", "-z", "--no-renames", "HEAD...@{u}"}, {_prefix.Empty() ? String(".") : _prefix.SubStr(0, _prefix.Size() - 1)}, output);
		if (result._success == false)
		{
			return result;
		}
		Vector<String> files;
		SplitNul(output, files);
		for (String& file : files)
		{
			_upstreamChanges.insert(std::move(file));
		}
		return Result();
	}

	/// @brief git lfs locks --verify: ours and the others', from the server
	/// @return
	VersionControlProvider::Result GitProvider::RefreshLocks()
	{
		if (_hasLfs == false || _usesLfs == false || _hasRemote == false)
		{
			_locks.clear();
			return Result();
		}

		Process::Result output;
		RunOptions      options;
		options._timeoutMs = 60 * 1000;
		Result result = Run({"lfs", "locks", "--verify", "--json"}, output, options);
		if (result._success == false)
		{
			return result;
		}

		Document           document;
		DocumentReaderJson reader;
		if (reader.Read(document, output._output.CStr(), output._output.Size()) == false)
		{
			return Result::Failure(fmt::format("git lfs locks: unexpected output \"{}\"", Trim(output._output)));
		}

		_locks.clear();
		for (const char* list : {"ours", "theirs"})
		{
			const DocumentNode* locks = document.GetRootNode().GetChild(list);
			if (locks == nullptr)
			{
				continue;
			}
			for (const DocumentNode* lock = locks->GetFirstChild(); lock != nullptr; lock = lock->GetNextSibling())
			{
				const DocumentNode* path = lock->GetChild("path");
				if (path == nullptr)
				{
					continue;
				}
				LockInfo info;
				info._ours = std::string_view(list) == "ours";
				const DocumentNode* owner = lock->GetChild("owner");
				const DocumentNode* name = owner != nullptr ? owner->GetChild("name") : nullptr;
				if (name != nullptr)
				{
					info._owner = name->GetString();
				}
				_locks[path->GetString()] = std::move(info);
			}
		}
		return Result();
	}

	/// @brief Fetch and lock list, at most once per _remoteRefreshDelay unless forced. Their failures are warned about once (offline,
	/// no LFS lock support on the server): the statuses are local only then
	/// @param force
	void GitProvider::RefreshRemoteInfo(bool force)
	{
		const std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		if (force == false && _remoteRefreshed && now - _lastRemoteRefresh < _remoteRefreshDelay)
		{
			return;
		}
		_lastRemoteRefresh = now;
		_remoteRefreshed = true;

		RefreshUpstream();
		if (_hasUpstream)
		{
			Result fetched = Fetch();
			if (fetched._success == false && _fetchWarned == false)
			{
				OUTPUT_WARNING("Git: can't fetch, the out of date files are the ones of the last fetch: {}", fetched._error);
				_fetchWarned = true;
			}
		}
		RefreshUpstreamChanges();

		Result locks = RefreshLocks();
		if (locks._success == false && _locksWarned == false)
		{
			OUTPUT_WARNING("Git: can't get the LFS locks: {}", locks._error);
			_locksWarned = true;
		}
	}

	/// @brief
	/// @param gitPaths
	/// @param files
	/// @return
	VersionControlProvider::Result GitProvider::ListTrackedFiles(const Vector<String>& gitPaths, Vector<String>& files) const
	{
		String output;
		Result result = RunChunked({"ls-files", "-z", "--full-name"}, gitPaths, output);
		if (result._success)
		{
			SplitNul(output, files);
		}
		return result;
	}

	/// @brief git check-attr lockable: "<path>\0lockable\0<set|unset|unspecified|value>\0" per file
	/// @param files
	/// @param lockable
	/// @return
	VersionControlProvider::Result GitProvider::FilterLockable(const Vector<String>& files, Vector<String>& lockable) const
	{
		if (files.Empty())
		{
			return Result();
		}

		const String input = JoinNul(files);
		RunOptions   options;
		options._input = input;
		Process::Result output;
		Result          result = Run({"check-attr", "-z", "--stdin", "lockable"}, output, options);
		if (result._success == false)
		{
			return result;
		}

		Vector<String> fields;
		SplitNul(output._output, fields);
		for (uint32_t index = 0; index + 2 < fields.Size(); index += 3)
		{
			if (fields[index + 2] == "set")
			{
				lockable.PushBack(fields[index]);
			}
		}
		return Result();
	}

	/// @brief Changed in the working tree or the index, or in a commit on no remote yet (unlocked, someone else could change it
	/// before the push)
	/// @param files
	/// @param changed
	/// @return
	VersionControlProvider::Result GitProvider::ListUnreleasable(const Vector<String>& files, std::unordered_set<String>& changed) const
	{
		StatusMap statuses;
		Result    result = QueryLocalStatus(files, statuses);
		if (result._success == false)
		{
			return result;
		}
		for (const auto& [path, status] : statuses)
		{
			if (status._state != VersionControlState::Unchanged && status._state != VersionControlState::Ignored)
			{
				changed.insert(ToGitPath(path));
			}
		}

		String output;
		result = RunChunked({"log", "-z", "--name-only", "--format=", "HEAD", "--not", "--remotes"}, files, output);
		if (result._success == false)
		{
			return result;
		}
		Vector<String> unpushed;
		SplitNul(output, unpushed);
		for (String& file : unpushed)
		{
			changed.insert(std::move(file));
		}
		return Result();
	}

	/// @brief Refused for a file changed upstream since its revision (just fetched) or locked by someone else
	/// @param files
	/// @return
	VersionControlProvider::Result GitProvider::LockFiles(const Vector<String>& files)
	{
		if (files.Empty())
		{
			return Result();
		}

		RefreshRemoteInfo(true);

		Vector<String> errors;
		for (const String& file : files)
		{
			auto it = _locks.find(file);
			if (it != _locks.end())
			{
				if (it->second._ours == false)
				{
					errors.PushBack(
						fmt::format("{} is locked by {}", file, it->second._owner.Empty() ? std::string_view("someone else") : std::string_view(it->second._owner)).c_str());
				}
				continue;
			}
			if (_upstreamChanges.contains(file))
			{
				errors.PushBack(fmt::format("{} changed on the server since your revision: pull first, a lock on it would lose these changes", file).c_str());
				continue;
			}

			Process::Result output;
			Result          result = Run({"lfs", "lock", file}, output);
			if (result._success)
			{
				_locks[file] = LockInfo {true, String()};
			}
			else
			{
				errors.PushBack(result._error);
			}
		}
		return errors.Empty() ? Result() : Result::Failure(JoinErrors(errors));
	}

	/// @brief
	/// @param files
	/// @return
	VersionControlProvider::Result GitProvider::MakeWritable(const Vector<String>& files)
	{
		Vector<String> errors;
		for (const String& file : files)
		{
			const Path      path = FromGitPath(file);
			std::error_code error;
			std::filesystem::permissions(std::filesystem::path(reinterpret_cast<const char8_t*>(path.GetString().CStr())), std::filesystem::perms::owner_write,
			                             std::filesystem::perm_options::add, error);
			if (error)
			{
				errors.PushBack(fmt::format("Can't make {} writable: {}", path, error.message()).c_str());
			}
		}
		return errors.Empty() ? Result() : Result::Failure(JoinErrors(errors));
	}

	/// @brief
	/// @param files
	/// @return
	VersionControlProvider::Result GitProvider::UnlockFiles(const Vector<String>& files)
	{
		Vector<String> errors;
		for (const String& file : files)
		{
			Process::Result output;
			Result          result = Run({"lfs", "unlock", file}, output);
			if (result._success)
			{
				_locks.erase(file);
			}
			else
			{
				errors.PushBack(result._error);
			}
		}
		return errors.Empty() ? Result() : Result::Failure(JoinErrors(errors));
	}

	/// @brief
	/// @param gitPaths
	/// @return
	VersionControlProvider::Result GitProvider::ReleaseUnchangedLocks(const Vector<String>& gitPaths)
	{
		Vector<String> candidates;
		for (const auto& [file, lock] : _locks)
		{
			if (lock._ours && IsUnderAny(file, gitPaths))
			{
				candidates.PushBack(file);
			}
		}
		if (candidates.Empty())
		{
			return Result();
		}

		std::unordered_set<String> changed;
		Result                     result = ListUnreleasable(candidates, changed);
		if (result._success == false)
		{
			return result;
		}

		Vector<String> toUnlock;
		for (const String& candidate : candidates)
		{
			if (changed.contains(candidate) == false)
			{
				toUnlock.PushBack(candidate);
			}
		}
		return UnlockFiles(toUnlock);
	}

	/// @brief In the project folder and its parents up to the root
	/// @param fileName
	/// @return
	bool GitProvider::FindUp(std::string_view fileName) const
	{
		uint32_t levels = 0;
		for (char character : _prefix)
		{
			levels += character == '/' ? 1 : 0;
		}

		Path directory = _projectDir;
		for (uint32_t level = 0; level <= levels; ++level)
		{
			if (FileSystem::GetInstance()->Exists(directory / Path(fileName)))
			{
				return true;
			}
			directory = directory.ParentPath();
		}
		return false;
	}

	/// @brief From the content of the .gitattributes of the project folder, of its parents and of .git/info (not those of its
	/// subfolders): enough to tell whether LFS and its locks are used
	void GitProvider::UpdateAttributes()
	{
		_usesLfs = false;
		_hasLockable = false;

		uint32_t levels = 0;
		for (char character : _prefix)
		{
			levels += character == '/' ? 1 : 0;
		}

		Vector<Path> files;
		Path         directory = _projectDir;
		for (uint32_t level = 0; level <= levels; ++level)
		{
			files.PushBack(directory / ".gitattributes");
			directory = directory.ParentPath();
		}
		files.PushBack(_root / ".git" / "info" / "attributes");

		for (const Path& file : files)
		{
			String content;
			if (FileSystem::GetInstance()->IsRegularFile(file) && FileSystem::GetInstance()->ReadAllText(file, content))
			{
				_usesLfs |= content.Find("filter=lfs") != String::Npos;
				_hasLockable |= content.Find("lockable") != String::Npos;
			}
		}
	}
}

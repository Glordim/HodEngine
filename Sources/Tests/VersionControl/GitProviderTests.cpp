#include <gtest/gtest.h>

#include <HodEngine/VersionControlEditor/Git/GitProvider.hpp>

#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Core/Process/Process.hpp>
#include <HodEngine/Core/UID.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>

using namespace hod;
using namespace hod::editor;

namespace
{
	using Capability = VersionControlProvider::Capability;
	using StatusMap = VersionControlProvider::StatusMap;

	bool RunGit(const Path& directory, const Vector<String>& arguments, String* output = nullptr)
	{
		Process::Desc desc;
		desc._program = "git";
		desc._arguments = arguments;
		desc._workingDirectory = directory;
		desc._environment.PushBack({"GIT_TERMINAL_PROMPT", "0"});
		Process::Result result;
		if (Process::Run(desc, result) == false)
		{
			return false;
		}
		if (output != nullptr)
		{
			*output = result._output;
		}
		if (result._exitCode != 0)
		{
			std::fprintf(stderr, "git failed: %s%s\n", result._output.CStr(), result._error.CStr());
		}
		return result._exitCode == 0;
	}

	bool IsGitInstalled()
	{
		Process::Desc desc;
		desc._program = "git";
		desc._arguments = {"--version"};
		Process::Result result;
		return Process::Run(desc, result) && result._exitCode == 0;
	}

	VersionControlState StateOf(const StatusMap& statuses, const Path& path)
	{
		auto it = statuses.find(path);
		return it != statuses.end() ? it->second._state : VersionControlState::Unchanged;
	}

	/// @brief A repository in a temporary folder, the project in its "Game" subfolder (a prefix in the working tree), with an asset,
	/// source and project settings folder
	class GitProviderTest : public ::testing::Test
	{
	protected:
		void SetUp() override
		{
			if (IsGitInstalled() == false)
			{
				GTEST_SKIP() << "git isn't installed";
			}
			_ownsFileSystem = FileSystem::GetInstance() == nullptr;
			if (_ownsFileSystem)
			{
				ASSERT_TRUE(FileSystem::CreateInstance()->Init());
			}

			_base = Path::Temp() / (String("HodGitProviderTests_") + UID::GenerateUID().ToString());
			_repository = _base / "Repository";
			_projectDir = _repository / "Game";
			ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(_projectDir / "Asset"));
			ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(_projectDir / "Source"));
			ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(_projectDir / "ProjectSettings"));
			ASSERT_TRUE(InitRepository(_repository));
		}

		void TearDown() override
		{
			_provider.Disconnect();
			if (_base.Empty() == false)
			{
				// Git's objects are read-only: made writable first, FileSystem::RemoveAll would leave them
				const std::string_view      utf8 = _base.GetString();
				const std::filesystem::path base(std::u8string(reinterpret_cast<const char8_t*>(utf8.data()), utf8.size()));
				std::error_code             error;
				for (const std::filesystem::directory_entry& entry : std::filesystem::recursive_directory_iterator(base, error))
				{
					std::filesystem::permissions(entry.path(), std::filesystem::perms::owner_write, std::filesystem::perm_options::add, error);
				}
				std::filesystem::remove_all(base, error);
			}
			if (_ownsFileSystem)
			{
				FileSystem::DestroyInstance();
			}
		}

		static bool InitRepository(const Path& directory)
		{
			return RunGit(directory, {"init", "-q", "-b", "main"}) && Configure(directory);
		}

		static bool Configure(const Path& directory)
		{
			return RunGit(directory, {"config", "user.name", "Hod Tests"}) && RunGit(directory, {"config", "user.email", "tests@hodengine.invalid"}) &&
			       RunGit(directory, {"config", "core.autocrlf", "false"}) && RunGit(directory, {"config", "commit.gpgsign", "false"});
		}

		static bool Write(const Path& path, const char* content)
		{
			return FileSystem::GetInstance()->WriteAllText(path, content);
		}

		/// @brief FileSystem opens the files with the ANSI functions on Windows: a non ASCII name is written through std::filesystem
		static bool WriteUtf8Name(const Path& path, const char* content)
		{
			const std::string_view      utf8 = path.GetString();
			const std::filesystem::path nativePath(std::u8string(reinterpret_cast<const char8_t*>(utf8.data()), utf8.size()));
			std::ofstream               file(nativePath, std::ios::binary);
			file << content;
			return file.good();
		}

		bool Commit(const char* message = "Commit")
		{
			return RunGit(_repository, {"add", "-A"}) && RunGit(_repository, {"commit", "-q", "-m", message});
		}

		StatusMap QueryAll()
		{
			StatusMap statuses;
			EXPECT_TRUE(_provider.QueryStatus(_rootFolders, statuses)._success);
			return statuses;
		}

		void Connect()
		{
			_rootFolders = {_projectDir / "Asset", _projectDir / "Source", _projectDir / "ProjectSettings"};
			VersionControlProvider::Result result = _provider.Connect(_projectDir);
			ASSERT_TRUE(result._success) << result._error.CStr();
		}

		/// @brief file committed "v1" and pushed to a bare remote (the upstream of main), then changed to "v2" by another clone
		void ChangeOnTheRemote(const Path& file)
		{
			const Path remote = _base / "Remote.git";
			const Path otherClone = _base / "Other";
			ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(remote));
			ASSERT_TRUE(RunGit(remote, {"init", "-q", "--bare", "-b", "main"}));

			ASSERT_TRUE(Write(file, "v1"));
			ASSERT_TRUE(Commit());
			ASSERT_TRUE(RunGit(_repository, {"remote", "add", "origin", remote.GetString()}));
			ASSERT_TRUE(RunGit(_repository, {"push", "-q", "-u", "origin", "main"}));

			ASSERT_TRUE(RunGit(_base, {"clone", "-q", remote.GetString(), otherClone.GetString()}));
			ASSERT_TRUE(Configure(otherClone));
			const String relative = file.GetString().SubStr(_repository.GetString().Size() + 1);
			ASSERT_TRUE(Write(otherClone / relative.CStr(), "v2"));
			ASSERT_TRUE(RunGit(otherClone, {"commit", "-q", "-a", "-m", "v2"}));
			ASSERT_TRUE(RunGit(otherClone, {"push", "-q"}));
		}

		static String Read(const Path& path)
		{
			String content;
			EXPECT_TRUE(FileSystem::GetInstance()->ReadAllText(path, content));
			return content;
		}

	protected:
		bool         _ownsFileSystem = false;
		Path         _base;
		Path         _repository;
		Path         _projectDir;
		Vector<Path> _rootFolders;
		GitProvider  _provider;
	};
}

TEST_F(GitProviderTest, Detect)
{
	EXPECT_TRUE(_provider.Detect(_projectDir));
	EXPECT_TRUE(_provider.Detect(_projectDir / "Asset"));

	const Path outside = _base / "NotVersioned";
	ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(outside));
	EXPECT_FALSE(_provider.Detect(outside));
}

TEST_F(GitProviderTest, ConnectOutOfARepository)
{
	const Path outside = _base / "NotVersioned";
	ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(outside));
	VersionControlProvider::Result result = _provider.Connect(outside);
	EXPECT_FALSE(result._success);
	EXPECT_FALSE(result._error.Empty());
}

TEST_F(GitProviderTest, ConnectCapabilities)
{
	Connect();
	const uint32_t capabilities = _provider.GetCapabilities();
	EXPECT_TRUE(capabilities & Capability::Diff);
	EXPECT_TRUE(capabilities & Capability::History);
	EXPECT_FALSE(capabilities & Capability::RemoteStatus); // No upstream
	EXPECT_FALSE(capabilities & Capability::Lock);         // No LFS in the attributes
	EXPECT_FALSE(capabilities & Capability::Checkout);
	EXPECT_FALSE(capabilities & Capability::TrackMoves);
	EXPECT_FALSE(capabilities & Capability::GetLatestFiles);
}

TEST_F(GitProviderTest, States)
{
	const Path unchanged = _projectDir / "Asset" / "Unchanged.texture";
	const Path modified = _projectDir / "Asset" / "Modified.texture";
	const Path deleted = _projectDir / "Asset" / "Deleted.texture";
	const Path renamed = _projectDir / "Asset" / "Renamed.texture";
	const Path ignored = _projectDir / "Asset" / "Ignored.tmp";
	ASSERT_TRUE(Write(unchanged, "a"));
	ASSERT_TRUE(Write(modified, "b"));
	ASSERT_TRUE(Write(deleted, "c"));
	ASSERT_TRUE(Write(renamed, "d"));
	ASSERT_TRUE(Write(_repository / ".gitignore", "*.tmp\n"));
	ASSERT_TRUE(Commit());

	const Path untracked = _projectDir / "Source" / "With space \xC3\xA9t\xC3\xA9.png";
	const Path added = _projectDir / "ProjectSettings" / "Added.json";
	const Path renamedTo = _projectDir / "Asset" / "Moved" / "Renamed.texture";
	ASSERT_TRUE(Write(modified, "changed"));
	ASSERT_TRUE(FileSystem::GetInstance()->Remove(deleted));
	ASSERT_TRUE(WriteUtf8Name(untracked, "e"));
	ASSERT_TRUE(Write(added, "f"));
	ASSERT_TRUE(Write(ignored, "g"));
	ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(renamedTo.ParentPath()));
	ASSERT_TRUE(RunGit(_repository, {"mv", "Game/Asset/Renamed.texture", "Game/Asset/Moved/Renamed.texture"}));

	Connect();
	ASSERT_TRUE(_provider.Add(Vector<Path> {added})._success);

	const StatusMap statuses = QueryAll();
	EXPECT_EQ(StateOf(statuses, unchanged), VersionControlState::Unchanged);
	EXPECT_EQ(StateOf(statuses, modified), VersionControlState::Modified);
	EXPECT_EQ(StateOf(statuses, deleted), VersionControlState::Deleted);
	EXPECT_EQ(StateOf(statuses, untracked), VersionControlState::Untracked);
	EXPECT_EQ(StateOf(statuses, added), VersionControlState::Added);
	EXPECT_EQ(StateOf(statuses, ignored), VersionControlState::Ignored);
	EXPECT_EQ(StateOf(statuses, renamedTo), VersionControlState::Renamed);
	EXPECT_EQ(StateOf(statuses, renamed), VersionControlState::Deleted);

	// The same, one file only
	StatusMap fileStatuses;
	ASSERT_TRUE(_provider.QueryFileStatus(Vector<Path> {modified}, fileStatuses)._success);
	EXPECT_EQ(fileStatuses.size(), 1u);
	EXPECT_EQ(StateOf(fileStatuses, modified), VersionControlState::Modified);
}

TEST_F(GitProviderTest, Conflicted)
{
	const Path file = _projectDir / "Asset" / "Conflict.scene";
	ASSERT_TRUE(Write(file, "base"));
	ASSERT_TRUE(Commit());
	ASSERT_TRUE(RunGit(_repository, {"checkout", "-q", "-b", "other"}));
	ASSERT_TRUE(Write(file, "other"));
	ASSERT_TRUE(Commit());
	ASSERT_TRUE(RunGit(_repository, {"checkout", "-q", "main"}));
	ASSERT_TRUE(Write(file, "main"));
	ASSERT_TRUE(Commit());
	EXPECT_FALSE(RunGit(_repository, {"merge", "-q", "other"}));

	Connect();
	EXPECT_EQ(StateOf(QueryAll(), file), VersionControlState::Conflicted);
}

TEST_F(GitProviderTest, PathsOutOfTheProjectIgnored)
{
	const Path outside = _repository / "Outside.texture";
	ASSERT_TRUE(Write(outside, "a"));
	Connect();

	ASSERT_TRUE(_provider.Add(Vector<Path> {outside})._success);
	String output;
	ASSERT_TRUE(RunGit(_repository, {"status", "--porcelain"}, &output));
	EXPECT_NE(std::string_view(output).find("?? Outside.texture"), std::string_view::npos); // Not added

	StatusMap statuses;
	ASSERT_TRUE(_provider.QueryFileStatus(Vector<Path> {outside}, statuses)._success);
	EXPECT_TRUE(statuses.empty());
}

TEST_F(GitProviderTest, Revert)
{
	const Path modified = _projectDir / "Asset" / "Modified.texture";
	const Path deleted = _projectDir / "Asset" / "Deleted.texture";
	ASSERT_TRUE(Write(modified, "original"));
	ASSERT_TRUE(Write(deleted, "original"));
	ASSERT_TRUE(Commit());

	const Path added = _projectDir / "Asset" / "Added.texture";
	ASSERT_TRUE(Write(modified, "changed"));
	ASSERT_TRUE(FileSystem::GetInstance()->Remove(deleted));
	ASSERT_TRUE(Write(added, "new"));

	Connect();
	ASSERT_TRUE(_provider.Add(Vector<Path> {added})._success);
	VersionControlProvider::Result result = _provider.Revert(Vector<Path> {_projectDir / "Asset"});
	ASSERT_TRUE(result._success) << result._error.CStr();

	String content;
	ASSERT_TRUE(FileSystem::GetInstance()->ReadAllText(modified, content));
	EXPECT_EQ(content, "original");
	EXPECT_TRUE(FileSystem::GetInstance()->Exists(deleted));
	EXPECT_TRUE(FileSystem::GetInstance()->Exists(added)); // Left on disk, untracked again

	const StatusMap statuses = QueryAll();
	EXPECT_EQ(StateOf(statuses, modified), VersionControlState::Unchanged);
	EXPECT_EQ(StateOf(statuses, deleted), VersionControlState::Unchanged);
	EXPECT_EQ(StateOf(statuses, added), VersionControlState::Untracked);
}

TEST_F(GitProviderTest, RevertWithoutCommit)
{
	const Path added = _projectDir / "Asset" / "Added.texture";
	ASSERT_TRUE(Write(added, "new"));
	Connect();
	ASSERT_TRUE(_provider.Add(Vector<Path> {added})._success);
	EXPECT_EQ(StateOf(QueryAll(), added), VersionControlState::Added);

	VersionControlProvider::Result result = _provider.Revert(Vector<Path> {added});
	ASSERT_TRUE(result._success) << result._error.CStr();
	EXPECT_EQ(StateOf(QueryAll(), added), VersionControlState::Untracked);
	EXPECT_TRUE(FileSystem::GetInstance()->Exists(added));
}

TEST_F(GitProviderTest, ManyPaths)
{
	// More than a command line can hold on Windows: chunked, or given on stdin
	Vector<Path> files;
	for (uint32_t index = 0; index < 1500; ++index)
	{
		Path file = _projectDir / "Asset" / String::Format("A rather long file name to fill the command line quickly %04u.texture", index);
		ASSERT_TRUE(Write(file, "x"));
		files.PushBack(std::move(file));
	}
	Connect();

	StatusMap statuses;
	ASSERT_TRUE(_provider.QueryFileStatus(files, statuses)._success);
	EXPECT_EQ(statuses.size(), files.Size());

	VersionControlProvider::Result result = _provider.Add(files);
	ASSERT_TRUE(result._success) << result._error.CStr();
	statuses.clear();
	ASSERT_TRUE(_provider.QueryFileStatus(files, statuses)._success);
	for (const Path& file : files)
	{
		EXPECT_EQ(StateOf(statuses, file), VersionControlState::Added);
	}
}

TEST_F(GitProviderTest, SetupWorkspaceCreatesWhatsMissing)
{
	Connect();
	VersionControlProvider::WorkspaceSetup setup;
	setup._ignoredFolders = {_projectDir / "Cache", _projectDir / "Build"};
	setup._ignoredFiles = {_projectDir / "UserSettings.json"};
	setup._binaryExtensions = {"texture", "png"};
	VersionControlProvider::Result result = _provider.SetupWorkspace(_projectDir, setup);
	ASSERT_TRUE(result._success) << result._error.CStr();

	String ignore;
	ASSERT_TRUE(FileSystem::GetInstance()->ReadAllText(_projectDir / ".gitignore", ignore));
	EXPECT_NE(ignore.Find("/Cache/\n"), String::Npos);
	EXPECT_NE(ignore.Find("/Build/\n"), String::Npos);
	EXPECT_NE(ignore.Find("/UserSettings.json\n"), String::Npos);

	String attributes;
	ASSERT_TRUE(FileSystem::GetInstance()->ReadAllText(_projectDir / ".gitattributes", attributes));
	if (_provider.HasLfs())
	{
		EXPECT_NE(attributes.Find("*.texture filter=lfs diff=lfs merge=lfs -text lockable\n"), String::Npos);
		EXPECT_NE(attributes.Find("*.png filter=lfs diff=lfs merge=lfs -text lockable\n"), String::Npos);
		EXPECT_TRUE(_provider.GetCapabilities() & Capability::Lock);
		EXPECT_TRUE(_provider.GetCapabilities() & Capability::Checkout);
	}
	else
	{
		EXPECT_NE(attributes.Find("*.texture binary\n"), String::Npos);
		EXPECT_FALSE(_provider.GetCapabilities() & Capability::Lock);
	}

	// What they ignore is ignored
	ASSERT_TRUE(FileSystem::GetInstance()->CreateDirectories(_projectDir / "Cache"));
	ASSERT_TRUE(Write(_projectDir / "Cache" / "Cooked.bin", "x"));
	ASSERT_TRUE(Write(_projectDir / "UserSettings.json", "{}"));
	String output;
	ASSERT_TRUE(RunGit(_repository, {"status", "--porcelain", "--untracked-files=all"}, &output));
	EXPECT_EQ(std::string_view(output).find("Cache"), std::string_view::npos);
	EXPECT_EQ(std::string_view(output).find("UserSettings.json"), std::string_view::npos);
}

TEST_F(GitProviderTest, SetupWorkspaceKeepsTheTeamsFiles)
{
	ASSERT_TRUE(Write(_repository / ".gitignore", "*.team\n"));       // At the root, above the project folder
	ASSERT_TRUE(Write(_projectDir / ".gitattributes", "*.x text\n")); // In the project folder
	Connect();

	VersionControlProvider::WorkspaceSetup setup;
	setup._ignoredFolders = {_projectDir / "Cache"};
	setup._binaryExtensions = {"texture"};
	ASSERT_TRUE(_provider.SetupWorkspace(_projectDir, setup)._success);

	EXPECT_FALSE(FileSystem::GetInstance()->Exists(_projectDir / ".gitignore"));
	String attributes;
	ASSERT_TRUE(FileSystem::GetInstance()->ReadAllText(_projectDir / ".gitattributes", attributes));
	EXPECT_EQ(attributes, "*.x text\n");
}

TEST_F(GitProviderTest, SetupWorkspaceWithoutLfsForCommittedBinaries)
{
	ASSERT_TRUE(Write(_projectDir / "Asset" / "Old.texture", "x"));
	ASSERT_TRUE(Commit());
	Connect();

	VersionControlProvider::WorkspaceSetup setup;
	setup._binaryExtensions = {"texture"};
	ASSERT_TRUE(_provider.SetupWorkspace(_projectDir, setup)._success);

	// LFS would change how the committed ones are stored: binary only
	String attributes;
	ASSERT_TRUE(FileSystem::GetInstance()->ReadAllText(_projectDir / ".gitattributes", attributes));
	EXPECT_NE(attributes.Find("*.texture binary\n"), String::Npos);
	EXPECT_EQ(attributes.Find("filter=lfs"), String::Npos);
	EXPECT_EQ(StateOf(QueryAll(), _projectDir / "Asset" / "Old.texture"), VersionControlState::Unchanged);
}

TEST_F(GitProviderTest, CheckoutWithoutLockableFiles)
{
	const Path file = _projectDir / "Asset" / "File.texture";
	ASSERT_TRUE(Write(file, "x"));
	ASSERT_TRUE(Commit());
	Connect();
	EXPECT_TRUE(_provider.Checkout(Vector<Path> {file})._success); // Nothing to do
	EXPECT_TRUE(_provider.RevertUnchanged(Vector<Path> {file})._success);
	EXPECT_FALSE(_provider.Lock(Vector<Path> {file})._success); // No LFS: not supported
}

TEST_F(GitProviderTest, OutOfDate)
{
	const Path file = _projectDir / "Asset" / "Shared.scene";
	const Path untouched = _projectDir / "Asset" / "Untouched.scene";
	ASSERT_TRUE(Write(untouched, "v1"));
	ChangeOnTheRemote(file); // Someone else changes it
	ASSERT_FALSE(HasFatalFailure());

	Connect();
	EXPECT_TRUE(_provider.GetCapabilities() & Capability::RemoteStatus);
	_provider.SetRemoteRefreshDelay(std::chrono::milliseconds(0));

	StatusMap statuses = QueryAll(); // Fetches
	ASSERT_TRUE(statuses.contains(file));
	EXPECT_TRUE(statuses.at(file)._outOfDate);
	EXPECT_EQ(statuses.at(file)._state, VersionControlState::Unchanged);
	EXPECT_FALSE(statuses.contains(untouched));

	// Once pulled, up to date
	ASSERT_TRUE(RunGit(_repository, {"pull", "-q", "--ff-only"}));
	statuses = QueryAll();
	EXPECT_FALSE(statuses.contains(file));
}

TEST_F(GitProviderTest, UpdateWorkspaceWithoutUpstream)
{
	ASSERT_TRUE(Write(_projectDir / "Asset" / "A.scene", "v1"));
	ASSERT_TRUE(Commit());
	Connect();
	EXPECT_FALSE(_provider.GetCapabilities() & Capability::UpdateWorkspace);
	EXPECT_FALSE(_provider.UpdateWorkspace(_rootFolders)._success);
}

TEST_F(GitProviderTest, UpdateWorkspace)
{
	const Path file = _projectDir / "Asset" / "Shared.scene";
	ChangeOnTheRemote(file);
	ASSERT_FALSE(HasFatalFailure());
	ASSERT_TRUE(Write(_projectDir / "Asset" / "Local.scene", "local")); // A local change elsewhere doesn't prevent it

	Connect();
	EXPECT_TRUE(_provider.GetCapabilities() & Capability::UpdateWorkspace);
	VersionControlProvider::Result result = _provider.UpdateWorkspace(_rootFolders);
	ASSERT_TRUE(result._success) << result._error.CStr();
	EXPECT_EQ(Read(file), "v2");
	EXPECT_EQ(Read(_projectDir / "Asset" / "Local.scene"), "local");

	StatusMap statuses;
	ASSERT_TRUE(_provider.QueryFileStatus(Vector<Path> {file}, statuses)._success); // Without a fetch: from the update itself
	EXPECT_FALSE(statuses.contains(file));
}

TEST_F(GitProviderTest, UpdateWorkspaceKeepsLocalChanges)
{
	const Path file = _projectDir / "Asset" / "Shared.scene";
	ChangeOnTheRemote(file);
	ASSERT_FALSE(HasFatalFailure());
	ASSERT_TRUE(Write(file, "mine"));

	Connect();
	EXPECT_FALSE(_provider.UpdateWorkspace(_rootFolders)._success); // Would overwrite it
	EXPECT_EQ(Read(file), "mine");
}

TEST_F(GitProviderTest, UpdateWorkspaceDiverged)
{
	const Path file = _projectDir / "Asset" / "Shared.scene";
	ChangeOnTheRemote(file);
	ASSERT_FALSE(HasFatalFailure());
	ASSERT_TRUE(Write(_projectDir / "Asset" / "Other.scene", "local"));
	ASSERT_TRUE(Commit("Local commit"));

	Connect();
	VersionControlProvider::Result result = _provider.UpdateWorkspace(_rootFolders);
	EXPECT_FALSE(result._success);
	EXPECT_NE(result._error.Find("merge or rebase"), String::Npos) << result._error.CStr();
	EXPECT_EQ(Read(file), "v1");

	StatusMap statuses;
	ASSERT_TRUE(_provider.QueryFileStatus(Vector<Path> {file}, statuses)._success);
	ASSERT_TRUE(statuses.contains(file));
	EXPECT_TRUE(statuses.at(file)._outOfDate);
}

TEST_F(GitProviderTest, SubmitOnlyTheseFiles)
{
	const Path modified = _projectDir / "Asset" / "Modified.scene";
	const Path untracked = _projectDir / "Asset" / "New.scene";
	const Path leftOut = _projectDir / "Asset" / "LeftOut.scene";
	const Path staged = _projectDir / "Asset" / "Staged.scene";
	ASSERT_TRUE(Write(modified, "v1"));
	ASSERT_TRUE(Write(leftOut, "v1"));
	ASSERT_TRUE(Commit());
	ASSERT_TRUE(Write(modified, "v2"));
	ASSERT_TRUE(Write(untracked, "new"));
	ASSERT_TRUE(Write(leftOut, "v2"));
	ASSERT_TRUE(Write(staged, "staged"));
	ASSERT_TRUE(RunGit(_repository, {"add", "--", "Game/Asset/Staged.scene"}));

	Connect();
	EXPECT_TRUE(_provider.GetCapabilities() & Capability::Submit);
	VersionControlProvider::Result result = _provider.Submit(Vector<Path> {modified, untracked}, "Two files\n\nWith a body: \"quotes\" & spaces");
	ASSERT_TRUE(result._success) << result._error.CStr();

	String message;
	ASSERT_TRUE(RunGit(_repository, {"log", "-1", "--format=%B"}, &message));
	EXPECT_NE(message.Find("With a body: \"quotes\" & spaces"), String::Npos) << message.CStr();
	String files;
	ASSERT_TRUE(RunGit(_repository, {"show", "--name-only", "--format=", "HEAD"}, &files));
	EXPECT_NE(files.Find("Game/Asset/Modified.scene"), String::Npos);
	EXPECT_NE(files.Find("Game/Asset/New.scene"), String::Npos);
	EXPECT_EQ(files.Find("LeftOut"), String::Npos);
	EXPECT_EQ(files.Find("Staged"), String::Npos);

	const StatusMap statuses = QueryAll();
	EXPECT_FALSE(statuses.contains(modified));
	EXPECT_FALSE(statuses.contains(untracked));
	EXPECT_EQ(StateOf(statuses, leftOut), VersionControlState::Modified);
	EXPECT_EQ(StateOf(statuses, staged), VersionControlState::Added); // Still staged
}

TEST_F(GitProviderTest, SubmitDeletedAndRenamed)
{
	const Path deleted = _projectDir / "Asset" / "Deleted.scene";
	const Path original = _projectDir / "Asset" / "Original.scene";
	const Path renamed = _projectDir / "Asset" / "Renamed.scene";
	ASSERT_TRUE(Write(deleted, "deleted"));
	ASSERT_TRUE(Write(original, "renamed"));
	ASSERT_TRUE(Commit());
	ASSERT_TRUE(FileSystem::GetInstance()->RemoveAll(deleted));
	ASSERT_TRUE(RunGit(_repository, {"mv", "Game/Asset/Original.scene", "Game/Asset/Renamed.scene"}));

	Connect();
	StatusMap statuses = QueryAll();
	ASSERT_EQ(StateOf(statuses, renamed), VersionControlState::Renamed);
	EXPECT_EQ(statuses.at(renamed)._originalPath, original);

	VersionControlProvider::Result result = _provider.Submit(Vector<Path> {deleted, renamed, original}, "Delete and rename");
	ASSERT_TRUE(result._success) << result._error.CStr();
	statuses = QueryAll();
	EXPECT_TRUE(statuses.empty());
}

TEST_F(GitProviderTest, Push)
{
	const Path file = _projectDir / "Asset" / "Shared.scene";
	ChangeOnTheRemote(file);
	ASSERT_FALSE(HasFatalFailure());
	ASSERT_TRUE(RunGit(_repository, {"pull", "-q", "--ff-only"}));

	Connect();
	EXPECT_TRUE(_provider.GetCapabilities() & Capability::Push);
	ASSERT_TRUE(Write(file, "v3"));
	ASSERT_TRUE(_provider.Submit(Vector<Path> {file}, "v3")._success);
	EXPECT_EQ(_provider.GetPendingPushCount(), 1u);

	VersionControlProvider::Result result = _provider.Push();
	ASSERT_TRUE(result._success) << result._error.CStr();
	EXPECT_EQ(_provider.GetPendingPushCount(), 0u);
	String remoteContent;
	ASSERT_TRUE(RunGit(_base / "Remote.git", {"show", "main:Game/Asset/Shared.scene"}, &remoteContent));
	EXPECT_NE(remoteContent.Find("v3"), String::Npos);
}

TEST_F(GitProviderTest, PushRejected)
{
	const Path file = _projectDir / "Asset" / "Shared.scene";
	ChangeOnTheRemote(file); // The remote has a commit we don't
	ASSERT_FALSE(HasFatalFailure());

	Connect();
	ASSERT_TRUE(Write(_projectDir / "Asset" / "Other.scene", "local"));
	ASSERT_TRUE(_provider.Submit(Vector<Path> {_projectDir / "Asset" / "Other.scene"}, "Local")._success);
	VersionControlProvider::Result result = _provider.Push();
	EXPECT_FALSE(result._success);
	EXPECT_NE(result._error.Find("Pull first"), String::Npos) << result._error.CStr();
}

TEST_F(GitProviderTest, CapabilityHints)
{
	ASSERT_TRUE(Write(_projectDir / "Asset" / "A.scene", "v1"));
	ASSERT_TRUE(Commit());
	Connect();
	EXPECT_FALSE(_provider.GetCapabilities() & Capability::Push); // No upstream
	EXPECT_FALSE(_provider.GetCapabilityHint(Capability::Push).Empty());
	EXPECT_FALSE(_provider.GetCapabilities() & Capability::Lock); // No LFS rules
	EXPECT_NE(_provider.GetCapabilityHint(Capability::Lock).Find("LFS"), String::Npos);
}

TEST_F(GitProviderTest, SubmitNonAsciiName)
{
	const Path file = _projectDir / "Asset" / "Ch\xC3\xA2teau.scene";
	ASSERT_TRUE(WriteUtf8Name(file, "v1"));
	ASSERT_TRUE(Commit());
	ASSERT_TRUE(WriteUtf8Name(file, "v2"));

	Connect();
	VersionControlProvider::Result result = _provider.Submit(Vector<Path> {file}, "Accent");
	ASSERT_TRUE(result._success) << result._error.CStr();
	String tracked;
	ASSERT_TRUE(RunGit(_repository, {"ls-files"}, &tracked));
	EXPECT_NE(tracked.Find("teau.scene"), String::Npos) << "the file was removed from the repository: " << tracked.CStr();
}

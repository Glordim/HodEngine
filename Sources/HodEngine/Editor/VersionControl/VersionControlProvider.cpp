#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlProvider.hpp"

namespace hod::inline editor
{
	/// @brief
	/// @param error
	/// @return
	VersionControlProvider::Result VersionControlProvider::Result::Failure(std::string_view error)
	{
		Result result;
		result._success = false;
		result._error = error;
		return result;
	}

	/// @brief
	/// @param operation
	/// @return
	VersionControlProvider::Result VersionControlProvider::NotSupported(std::string_view operation) const
	{
		return Result::Failure(String::Format("%.*s can't %.*s", static_cast<int>(GetName().size()), GetName().data(), static_cast<int>(operation.size()), operation.data()));
	}

	/// @brief
	void VersionControlProvider::Disconnect() {}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::SetupWorkspace(const Path& projectDir, const WorkspaceSetup& setup)
	{
		(void)projectDir;
		(void)setup;
		return Result();
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::RevertUnchanged(ArrayView<Path> paths)
	{
		(void)paths;
		return Result();
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::Checkout(ArrayView<Path> paths)
	{
		(void)paths;
		return Result();
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::Lock(ArrayView<Path> paths)
	{
		(void)paths;
		return NotSupported("lock");
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::Unlock(ArrayView<Path> paths)
	{
		(void)paths;
		return NotSupported("unlock");
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::GetLatest(ArrayView<Path> paths)
	{
		(void)paths;
		return NotSupported("get the latest revision of some files only");
	}

	/// @brief
	/// @param folders
	/// @return
	VersionControlProvider::Result VersionControlProvider::UpdateWorkspace(ArrayView<Path> folders)
	{
		(void)folders;
		return NotSupported("update the workspace");
	}

	/// @brief
	/// @param paths
	/// @param message
	/// @return
	VersionControlProvider::Result VersionControlProvider::Submit(ArrayView<Path> paths, std::string_view message)
	{
		(void)paths;
		(void)message;
		return NotSupported("submit");
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::Push()
	{
		return NotSupported("push");
	}

	/// @brief
	/// @return
	uint32_t VersionControlProvider::GetPendingPushCount() const
	{
		return 0;
	}

	/// @brief
	/// @param capability
	/// @return
	String VersionControlProvider::GetCapabilityHint(uint32_t capability) const
	{
		(void)capability;
		return String::Format("%.*s can't do it", static_cast<int>(GetName().size()), GetName().data());
	}

	/// @brief
	/// @param projectDir
	/// @return
	Vector<Path> VersionControlProvider::GetWorkspaceFiles(const Path& projectDir) const
	{
		(void)projectDir;
		return Vector<Path>();
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::Move(const Path& from, const Path& to)
	{
		(void)from;
		(void)to;
		return NotSupported("move");
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::Delete(ArrayView<Path> paths)
	{
		(void)paths;
		return NotSupported("delete");
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::OpenDiff(const Path& path)
	{
		(void)path;
		return NotSupported("open a diff");
	}

	/// @brief
	/// @return
	VersionControlProvider::Result VersionControlProvider::OpenHistory(const Path& path)
	{
		(void)path;
		return NotSupported("open the history");
	}
}

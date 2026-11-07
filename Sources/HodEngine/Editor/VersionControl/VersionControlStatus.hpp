#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/Core/String.hpp>

#include <cstdint>

namespace hod::inline editor
{
	/// @brief Local state of a file (or folder) for the version control
	enum class VersionControlState : uint8_t
	{
		Unknown,    // Not refreshed yet, or no provider connected
		Ignored,    // .gitignore, svn:ignore, p4 ignore
		Untracked,  // On disk only: not added
		Unchanged,  // Tracked, same as its revision (read-only on disk with Perforce until checked out)
		Added,      // Added, not submitted yet
		Modified,   // Git/Svn: changed on disk. Perforce: checked out (opened for edit)
		Deleted,    // Deleted, not submitted yet
		Renamed,    // Moved/renamed, not submitted yet (its new path)
		Conflicted, // Merge/rebase/resolve pending
	};

	/// @brief What the asset browser shows of a file: its state, plus what the others do with it
	struct HOD_EDITOR_API VersionControlStatus
	{
		VersionControlState _state = VersionControlState::Unknown;

		bool   _outOfDate = false;     // A newer revision on the server (see VersionControlProvider::Capability::RemoteStatus)
		bool   _lockedByMe = false;    // p4 exclusive checkout / svn lock / git lfs lock, by the current user
		bool   _lockedByOther = false; // Same by someone else: can't be checked out nor written
		String _otherUsers;            // Who else has it opened/locked, for the tooltip ("john@john-pc, ...")
		Path   _originalPath;          // Renamed: where it comes from (Deleted there), submitted with it. Not combined

		/// @brief Of an asset (its .asset and its sources) or of a folder (what's under it): the most important state wins
		/// (Conflicted > Deleted/Renamed/Added/Modified > Untracked > Unchanged > Ignored > Unknown), the flags are OR-ed
		/// and the users concatenated
		static VersionControlStatus Combine(const VersionControlStatus& left, const VersionControlStatus& right);
	};
}

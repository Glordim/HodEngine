#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/VersionControl/VersionControlStatus.hpp"

namespace hod::inline editor
{
	namespace
	{
		/// @brief The order Combine keeps
		uint32_t GetPriority(VersionControlState state)
		{
			switch (state)
			{
				case VersionControlState::Unknown: return 0;
				case VersionControlState::Ignored: return 1;
				case VersionControlState::Unchanged: return 2;
				case VersionControlState::Untracked: return 3;
				case VersionControlState::Modified: return 4;
				case VersionControlState::Added: return 5;
				case VersionControlState::Renamed: return 6;
				case VersionControlState::Deleted: return 7;
				case VersionControlState::Conflicted: return 8;
			}
			return 0;
		}

		/// @brief users is a ", " separated list
		void AppendUsers(String& users, const String& others)
		{
			if (others.Empty() || users == others)
			{
				return;
			}
			if (users.Empty())
			{
				users = others;
				return;
			}
			// A list of a few names: the duplicates are looked for as they are
			std::string_view remaining = others;
			while (remaining.empty() == false)
			{
				const size_t     separator = remaining.find(", ");
				std::string_view user = remaining.substr(0, separator);
				remaining = separator == std::string_view::npos ? std::string_view() : remaining.substr(separator + 2);

				bool             found = false;
				std::string_view existing = users;
				while (existing.empty() == false && found == false)
				{
					const size_t existingSeparator = existing.find(", ");
					found = existing.substr(0, existingSeparator) == user;
					existing = existingSeparator == std::string_view::npos ? std::string_view() : existing.substr(existingSeparator + 2);
				}
				if (found == false)
				{
					users += ", ";
					users += user;
				}
			}
		}
	}

	/// @brief
	/// @param left
	/// @param right
	/// @return
	VersionControlStatus VersionControlStatus::Combine(const VersionControlStatus& left, const VersionControlStatus& right)
	{
		VersionControlStatus result;
		result._state = GetPriority(left._state) >= GetPriority(right._state) ? left._state : right._state;
		result._outOfDate = left._outOfDate || right._outOfDate;
		result._lockedByMe = left._lockedByMe || right._lockedByMe;
		result._lockedByOther = left._lockedByOther || right._lockedByOther;
		result._otherUsers = left._otherUsers;
		AppendUsers(result._otherUsers, right._otherUsers);
		return result;
	}
}

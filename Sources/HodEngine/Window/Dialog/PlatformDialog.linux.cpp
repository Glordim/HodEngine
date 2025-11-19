#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

namespace hod::window
{
	/// @brief
	/// @return
	Path GetOpenFileDialog()
	{
		char buffer[4096] = {'\0'};

		// TODO

		return buffer;
	}

	/// @brief
	/// @return
	Path GetSaveFileDialog(const std::string_view& /*typeName*/, const std::string_view& /*typeExtension*/, const Path& /*initialFolder*/)
	{
		char buffer[4096] = {'\0'};

		// TODO

		return buffer;
	}

	/// @brief
	/// @return
	Path GetFolderDialog()
	{
		// TODO

		return Path();
	}

	/// @brief
	/// @param path
	void OpenExplorerAtPath(const Path& /*path*/)
	{
		// TODO
	}

	/// @brief
	/// @param title
	/// @param description
	/// @param yesChoice
	/// @param noChoice
	/// @return
	bool ShowYesNoDialog(const std::string_view& /*title*/, const std::string_view& /*description*/, const std::string_view& /*yesChoice*/, const std::string_view& /*noChoice*/)
	{
		return true; // todo
	}
}

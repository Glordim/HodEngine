#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Core/FileSystem/FileSystem.hpp>
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include <Windows.h>
#include <shlobj_core.h>

namespace hod::window
{
	/// @brief
	/// @return
	Path GetOpenFileDialog()
	{
		char buffer[4096] = {'\0'};

		OPENFILENAMEA openFileName;
		ZeroMemory(&openFileName, sizeof(openFileName));
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = NULL;
		openFileName.lpstrFile = buffer;
		openFileName.nMaxFile = sizeof(buffer);
		openFileName.lpstrFilter = "Hod Project\0*.hod\0";
		openFileName.nFilterIndex = 1;
		openFileName.lpstrFileTitle = NULL;
		openFileName.nMaxFileTitle = 0;
		openFileName.lpstrInitialDir = NULL;
		openFileName.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		GetOpenFileNameA(&openFileName);

		return buffer;
	}

	/// @brief
	/// @return
	Path GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const Path& initialFolder)
	{
		char buffer[4096] = {'\0'};

		strcpy(buffer, "EditMe.");
		strcat(buffer, typeExtension.data());

		char filter[4096] = {'\0'};
		strcpy(filter, typeName.data());
		strcpy(filter + typeName.size() + 1, typeExtension.data());

		String initialFolderPath = initialFolder.GetString().CStr();

		OPENFILENAMEA openFileName;
		ZeroMemory(&openFileName, sizeof(openFileName));
		openFileName.lStructSize = sizeof(openFileName);
		openFileName.hwndOwner = NULL;
		openFileName.lpstrFile = buffer;
		openFileName.nMaxFile = sizeof(buffer);
		openFileName.lpstrFilter = filter;
		openFileName.nFilterIndex = 1;
		openFileName.lpstrFileTitle = NULL;
		openFileName.nMaxFileTitle = 0;
		if (initialFolder.Empty() == true)
		{
			openFileName.lpstrInitialDir = NULL;
		}
		else
		{
			openFileName.lpstrInitialDir = initialFolderPath.CStr();
		}
		openFileName.Flags = 0; // OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		GetSaveFileNameA(&openFileName);

		return buffer;
	}

	/// @brief
	/// @return
	Path GetFolderDialog()
	{
		char        buffer[MAX_PATH] = {'\0'};
		BROWSEINFOA browseInfo;
		browseInfo.hwndOwner = NULL;
		browseInfo.pidlRoot = NULL;
		browseInfo.pszDisplayName = buffer;
		browseInfo.lpszTitle = "Select a folder";
		browseInfo.ulFlags = BIF_NEWDIALOGSTYLE;
		browseInfo.lpfn = NULL;
		browseInfo.lParam = 0;
		browseInfo.iImage = -1;
		LPITEMIDLIST itemIdLists = SHBrowseForFolderA(&browseInfo);
		if (itemIdLists != NULL)
		{
			CHAR dirPaths[MAX_PATH];
			SHGetPathFromIDListA(itemIdLists, dirPaths);
			CoTaskMemFree(itemIdLists);
			return dirPaths;
		}

		return Path();
	}

	/// @brief
	/// @param path
	void OpenExplorerAtPath(const Path& path)
	{
		if (FileSystem::GetInstance()->IsDirectory(path))
		{
			ShellExecute(NULL, "open", path.GetString().CStr(), NULL, NULL, SW_SHOWDEFAULT);
		}
		else if (FileSystem::GetInstance()->IsRegularFile(path))
		{
			String param = String::Format("/select,\"%s\"", path.GetString().CStr());
			ShellExecute(NULL, "open", "explorer.exe", param.CStr(), NULL, SW_SHOWDEFAULT);
		}
		else
		{
			// TODO
		}
	}

	/// @brief
	/// @param title
	/// @param description
	/// @param yesChoice
	/// @param noChoice
	/// @return
	bool ShowYesNoDialog(const std::string_view& title, const std::string_view& description, const std::string_view& /*yesChoice*/,
	                     [[maybe_unused]] const std::string_view& /*noChoice*/)
	{
		return MessageBox(NULL, description.data(), title.data(), MB_YESNO) == IDYES;
	}
}

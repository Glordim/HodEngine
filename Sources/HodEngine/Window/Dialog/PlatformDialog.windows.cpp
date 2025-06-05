#include "HodEngine/Window/Pch.hpp"
#include <HodEngine/Window/Dialog/PlatformDialog.hpp>

#include <Windows.h>
#include <shlobj_core.h>

namespace hod::window
{
	/// @brief 
	/// @return 
	std::filesystem::path GetOpenFileDialog()
	{
		char buffer[4096] = { '\0' };

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
	std::filesystem::path GetSaveFileDialog(const std::string_view& typeName, const std::string_view& typeExtension, const std::filesystem::path& initialFolder)
	{
		char buffer[4096] = { '\0' };

		strcpy(buffer, "EditMe.");
		strcat(buffer, typeExtension.data());

		char filter[4096] = { '\0' };
		strcpy(filter, typeName.data());
		strcpy(filter + typeName.size() + 1, typeExtension.data());

		String initialFolderPath = initialFolder.string();

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
		if (initialFolder.empty() == true)
		{
			openFileName.lpstrInitialDir = NULL;
		}
		else
		{
			openFileName.lpstrInitialDir = initialFolderPath.c_str();
		}
		openFileName.Flags = 0;//OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		GetSaveFileNameA(&openFileName);

		return buffer;
	}

	/// @brief 
	/// @return 
	std::filesystem::path GetFolderDialog()
	{
		char buffer[MAX_PATH] = { '\0' }; 
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

		return std::filesystem::path();
	}

	/// @brief 
	/// @param path 
	void OpenExplorerAtPath(const std::filesystem::path& path)
	{
		ShellExecute(NULL, "open", path.string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
	}

	/// @brief 
	/// @param title 
	/// @param description 
	/// @param yesChoice 
	/// @param noChoice 
	/// @return 
	bool ShowYesNoDialog(const std::string_view& title, const std::string_view& description, const std::string_view& yesChoice, const std::string_view& noChoice)
	{
		return MessageBox(NULL, description.data(), title.data(), MB_YESNO) == IDYES;
	}
}

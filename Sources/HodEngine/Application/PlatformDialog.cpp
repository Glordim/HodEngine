#include <HodEngine/Application/PlatformDialog.h>

#include <Windows.h>
#include <shlobj_core.h>

namespace hod::application::dialog
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
	std::filesystem::path GetSaveFileDialog()
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
		STARTUPINFOA startupInfo;
		ZeroMemory(&startupInfo, sizeof(startupInfo));
		startupInfo.cb = sizeof(startupInfo);

		PROCESS_INFORMATION processInformation;
		ZeroMemory(&processInformation, sizeof(processInformation));

		CreateProcessA(NULL, (LPSTR)(std::string("explorer.exe ") + path.string()).c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &startupInfo, &processInformation);
	}
}

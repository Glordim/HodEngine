#include <PlatformDialog.h>

#include <Windows.h>

namespace hod::application::dialog
{
	/// @brief 
	/// @return 
	std::filesystem::path GetOpenFileDialog()
	{
		char buffer[4096] = { '\0' };

		OPENFILENAME openFileName;
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

		GetOpenFileName(&openFileName);

		return buffer;
	}

	/// @brief 
	std::filesystem::path GetSaveFileDialog()
	{
		char buffer[4096] = { '\0' };

		OPENFILENAME openFileName;
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

		GetSaveFileName(&openFileName);

		return buffer;
	}
}

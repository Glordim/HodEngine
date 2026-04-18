#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

#undef CopyFile

namespace hod::inline core
{
	/// @brief
	/// @param path
	/// @param copyForSupportReload
	void DynamicLibrary::Init(const Path& path, bool copyForSupportReload)
	{
		_path = path;
		_copyForSupportReload = copyForSupportReload;

		if (_path.HasExtension() == false)
		{
			_path += GetModuleExtension();
		}
		else
		{
			_path.ReplaceExtension(GetModuleExtension());
		}

		_path.ReplaceFilename(GetModulePrefix() + _path.Filename().GetString());

		_copyPath = _path;
		if (_copyForSupportReload == true)
		{
			Path newFilename = _copyPath.Stem();
			newFilename += "-Copy";
			newFilename += _copyPath.Extension();
			_copyPath.ReplaceFilename(newFilename);
		}
	}

	/// @brief
	DynamicLibrary::~DynamicLibrary()
	{
		Unload();
	}

	/// @brief
	/// @return
	const Path& DynamicLibrary::GetPath() const
	{
		return _path;
	}

	/// @brief
	/// @return
	bool DynamicLibrary::Load()
	{
		if (_copyForSupportReload == true)
		{
			try
			{
				FileSystem::GetInstance()->CopyFile(_path, _copyPath, true);
			}
			catch (std::exception& e)
			{
				OUTPUT_ERROR("DynamicLibrary::Load(): {}", e.what());
			}
			return InternalLoad(_copyPath);
		}
		else
		{
			return InternalLoad(_path);
		}
	}

	/// @brief
	/// @return
	bool DynamicLibrary::Unload()
	{
		if (InternalUnload() == false)
		{
			return false;
		}

		if (_copyForSupportReload == true && FileSystem::GetInstance()->Exists(_copyPath))
		{
			try
			{
				FileSystem::GetInstance()->Remove(_copyPath);
			}
			catch (...)
			{
			}
		}

		return true;
	}

	/// @brief
	/// @return
	bool DynamicLibrary::Reload()
	{
		if (Unload() == false)
		{
			return false;
		}
		if (Load() == false)
		{
			return false;
		}
		return true;
	}
}

#include "HodEngine/Core/Module/Module.hpp"

namespace hod
{
	/// @brief 
	/// @param path 
	/// @param copyForSupportReload 
	Module::Module(const std::filesystem::path& path, bool copyForSupportReload)
	: _path(path)
	, _copyForSupportReload(copyForSupportReload)
	{
		if (_path.has_extension() == false)
		{
			_path += GetModuleExtension();
		}
		else
		{
			_path.replace_extension(GetModuleExtension());
		}

		_copyPath = _path;
		if (_copyForSupportReload == true)
		{
			std::filesystem::path newFilename = _copyPath.stem();
			newFilename += "-Copy";
			newFilename += _copyPath.extension();
			_copyPath.replace_filename(newFilename);
		}
	}

	/// @brief 
	/// @return 
	const std::filesystem::path& Module::GetPath() const
	{
		return _path;
	}

	/// @brief 
	/// @return 
	bool Module::Load()
	{
		if (_copyForSupportReload == true)
		{
			std::filesystem::copy_file(_path, _copyPath, std::filesystem::copy_options::update_existing);
			return InternalLoad(_copyPath);
		}
		else
		{
			return InternalLoad(_path);
		}
	}

	/// @brief 
	/// @return 
	bool Module::Unload()
	{
		if (InternaUnload() == false)
		{
			return false;
		}

		if (_copyForSupportReload == true && std::filesystem::exists(_copyPath))
		{
			std::filesystem::remove(_copyPath);
		}

		return true;
	}

	/// @brief 
	/// @return 
	bool Module::Reload()
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

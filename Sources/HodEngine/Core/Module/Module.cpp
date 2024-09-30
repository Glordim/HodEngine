#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Module/Module.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod
{
	/// @brief 
	/// @param path 
	/// @param copyForSupportReload 
	void Module::Init(const std::filesystem::path& path, bool copyForSupportReload)
	{
		_path = path;
		_copyForSupportReload = copyForSupportReload;

		if (_path.has_extension() == false)
		{
			_path += GetModuleExtension();
		}
		else
		{
			_path.replace_extension(GetModuleExtension());
		}

		_path.replace_filename(GetModulePrefix() + _path.filename().string());

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
	Module::~Module()
	{
		Unload();
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
			try
			{
				std::filesystem::copy_file(_path, _copyPath, std::filesystem::copy_options::update_existing);
			}
			catch (std::exception& e)
			{
				OUTPUT_ERROR("Module::Load(): {}", e.what());
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
	bool Module::Unload()
	{
		if (InternalUnload() == false)
		{
			return false;
		}

		if (_copyForSupportReload == true && std::filesystem::exists(_copyPath))
		{
			try
			{
				std::filesystem::remove(_copyPath);
			}
			catch (...)
			{

			}
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

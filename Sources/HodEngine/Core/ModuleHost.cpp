#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/ModuleHost.hpp"

#include "HodEngine/Core/DynamicLibrary/DynamicLibrary.hpp"
#include "HodEngine/Core/Module.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod::inline core
{
	/// @brief
	ModuleHost::~ModuleHost()
	{
		StopAll();
	}

	/// @brief
	/// @param name
	/// @param startup
	/// @return
	bool ModuleHost::Start(const String& name, ModuleStartup startup)
	{
		ModuleRegistration* registration = DefaultAllocator::GetInstance().New<ModuleRegistration>();
		if (startup(*registration) == false)
		{
			OUTPUT_ERROR("Module '{}': its startup failed", name);
			DefaultAllocator::GetInstance().Delete(registration); // What it registered before failing
			return false;
		}
		_entries.PushBack({name, registration, nullptr});
		return true;
	}

	/// @brief
	/// @param name
	/// @param path
	/// @param copyForReload
	/// @return
	bool ModuleHost::Load(const String& name, const Path& path, bool copyForReload)
	{
		DynamicLibrary* library = DefaultAllocator::GetInstance().New<DynamicLibrary>();
		library->Init(path, copyForReload);
		if (library->Load() == false)
		{
			OUTPUT_ERROR("Module '{}': its library {} can't be loaded", name, library->GetPath());
			DefaultAllocator::GetInstance().Delete(library);
			return false;
		}

		ModuleStartup startup = library->LoadFunction<ModuleStartup>(HOD_MODULE_STARTUP_FUNCTION_NAME);
		if (startup == nullptr)
		{
			OUTPUT_ERROR("Module '{}': no entry point " HOD_MODULE_STARTUP_FUNCTION_NAME " in its library {} (see HOD_STARTUP_MODULE)", name, library->GetPath());
			DefaultAllocator::GetInstance().Delete(library);
			return false;
		}

		if (Start(name, startup) == false)
		{
			DefaultAllocator::GetInstance().Delete(library); // What it registered undone already, while its code was loaded
			return false;
		}
		_entries.Back()._library = library;
		return true;
	}

	/// @brief
	/// @param count
	void ModuleHost::StopLast(uint32_t count)
	{
		for (; count > 0 && _entries.Empty() == false; --count)
		{
			Entry& entry = _entries.Back();
			DefaultAllocator::GetInstance().Delete(entry._registration); // While its code is still loaded
			DefaultAllocator::GetInstance().Delete(entry._library);
			_entries.PopBack();
		}
	}

	/// @brief
	void ModuleHost::StopAll()
	{
		StopLast(_entries.Size());
	}

	/// @brief
	/// @return
	uint32_t ModuleHost::GetModuleCount() const
	{
		return _entries.Size();
	}
}

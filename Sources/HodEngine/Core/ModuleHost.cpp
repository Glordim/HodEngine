#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/ModuleHost.hpp"

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
		_entries.PushBack({name, registration});
		return true;
	}

	/// @brief
	/// @param count
	void ModuleHost::StopLast(uint32_t count)
	{
		for (; count > 0 && _entries.Empty() == false; --count)
		{
			DefaultAllocator::GetInstance().Delete(_entries.Back()._registration);
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

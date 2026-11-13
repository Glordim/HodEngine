#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/RuntimeModule.hpp"

namespace hod::inline game
{
	/// @brief
	RuntimeModuleHost::~RuntimeModuleHost()
	{
		UnregisterAll();
	}

	/// @brief
	/// @param module
	void RuntimeModuleHost::Register(const RuntimeModule& module)
	{
		Entry entry;
		entry._module = &module;
		entry._registration = DefaultAllocator::GetInstance().New<ModuleRegistration>();
		if (module._register != nullptr)
		{
			module._register(*entry._registration);
		}
		_entries.PushBack(entry);
	}

	/// @brief
	/// @return
	bool RuntimeModuleHost::InitAll()
	{
		for (Entry& entry : _entries)
		{
			if (entry._initialized)
			{
				continue;
			}
			if (entry._module->_init != nullptr && entry._module->_init() == false)
			{
				OUTPUT_ERROR("Runtime module '{}': Init failed", entry._module->_name);
				ShutdownAll();
				return false;
			}
			entry._initialized = true;
		}
		return true;
	}

	/// @brief
	void RuntimeModuleHost::ShutdownAll()
	{
		for (auto it = _entries.RBegin(); it != _entries.REnd(); ++it)
		{
			if (it->_initialized)
			{
				if (it->_module->_shutdown != nullptr)
				{
					it->_module->_shutdown();
				}
				it->_initialized = false;
			}
		}
	}

	/// @brief
	void RuntimeModuleHost::UnregisterAll()
	{
		ShutdownAll();
		for (auto it = _entries.RBegin(); it != _entries.REnd(); ++it)
		{
			DefaultAllocator::GetInstance().Delete(it->_registration); // Unregisters what the module registered
		}
		_entries.Clear();
	}

	/// @brief
	/// @return
	uint32_t RuntimeModuleHost::GetModuleCount() const
	{
		return _entries.Size();
	}
}

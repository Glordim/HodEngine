#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/RuntimeModule.hpp"
#include "HodEngine/Game/WorldSystem.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline game
{
	/// @brief
	RuntimeModuleRegistration::~RuntimeModuleRegistration()
	{
		UnregisterAll();
	}

	/// @brief
	/// @param descriptor
	void RuntimeModuleRegistration::RegisterComponent(ReflectionDescriptor& descriptor)
	{
		if (ComponentFactory::GetInstance()->Register(descriptor))
		{
			_components.PushBack(&descriptor);
		}
	}

	/// @brief
	/// @param type
	/// @return
	bool RuntimeModuleRegistration::RegisterWorldSystem(const WorldSystemType& type)
	{
		if (WorldSystemRegistry::Register(type) == false)
		{
			return false;
		}
		_worldSystems.PushBack(&type);
		return true;
	}

	/// @brief
	/// @param unregister
	void RuntimeModuleRegistration::AddUnregister(void (*unregister)())
	{
		_unregisters.PushBack(unregister);
	}

	/// @brief
	void RuntimeModuleRegistration::UnregisterAll()
	{
		for (auto it = _unregisters.RBegin(); it != _unregisters.REnd(); ++it)
		{
			(*it)();
		}
		_unregisters.Clear();

		for (auto it = _worldSystems.RBegin(); it != _worldSystems.REnd(); ++it)
		{
			WorldSystemRegistry::Unregister(**it);
		}
		_worldSystems.Clear();

		ComponentFactory* componentFactory = ComponentFactory::GetInstance();
		for (auto it = _components.RBegin(); it != _components.REnd(); ++it)
		{
			if (componentFactory != nullptr)
			{
				componentFactory->Unregister(**it);
			}
		}
		_components.Clear();
	}

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
		entry._registration = DefaultAllocator::GetInstance().New<RuntimeModuleRegistration>();
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
			DefaultAllocator::GetInstance().Delete(it->_registration);
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

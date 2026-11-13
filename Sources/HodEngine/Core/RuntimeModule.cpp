#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/RuntimeModule.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param init
	/// @param shutdown
	void RuntimeModuleRegistration::AddInit(std::function<bool()> init, std::function<void()> shutdown)
	{
		_inits.PushBack({std::move(init), std::move(shutdown)});
	}

	/// @brief
	RuntimeModuleHost::~RuntimeModuleHost()
	{
		UnregisterAll();
	}

	/// @brief
	/// @param name
	/// @param startup
	void RuntimeModuleHost::Register(const String& name, RuntimeModuleStartup startup)
	{
		Entry entry;
		entry._name = name;
		entry._registration = DefaultAllocator::GetInstance().New<RuntimeModuleRegistration>();
		startup(*entry._registration);
		_entries.PushBack(entry);
	}

	/// @brief
	/// @return
	bool RuntimeModuleHost::InitAll()
	{
		for (Entry& entry : _entries)
		{
			Vector<RuntimeModuleRegistration::Init>& inits = entry._registration->_inits;
			while (entry._initCount < inits.Size())
			{
				RuntimeModuleRegistration::Init& init = inits[entry._initCount];
				++entry._initCount;
				if (init._init() == false)
				{
					OUTPUT_ERROR("Runtime module '{}': Init failed", entry._name);
					return false;
				}
				if (init._shutdown != nullptr)
				{
					entry._registration->AddUnregister(std::move(init._shutdown));
				}
			}
		}
		return true;
	}

	/// @brief
	void RuntimeModuleHost::UnregisterAll()
	{
		for (auto it = _entries.RBegin(); it != _entries.REnd(); ++it)
		{
			DefaultAllocator::GetInstance().Delete(it->_registration); // Its shutdowns, then what it registered
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

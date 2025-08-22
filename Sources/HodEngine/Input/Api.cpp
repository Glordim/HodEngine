#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/Devices/Device.hpp"
#include "HodEngine/Input/InputManager.hpp"

namespace hod::input
{
	/// @brief 
	/// @param name 
	Api::Api(const std::string_view& name)
	: _name(name)
	{
	}

	/// @brief 
	/// @param device 
	/// @param connected 
	void Api::SetDeviceConnected(Device* device, bool connected)
	{
		device->SetConnected(connected);
	}

	/// @brief 
	/// @param device 
	void Api::NotifyDeviceConnected(Device* device)
	{
		// TODO
		(void)device;
		//InputManager::GetInstance()->NotifyDeviceConnected(pDevice);
	}

	/// @brief 
	/// @param device 
	void Api::NotifyDeviceDisconnected(Device* device)
	{
		// TODO
		(void)device;
		//InputManager::GetInstance()->NotifyDeviceDisconnected(pDevice);
	}

	/// @brief 
	/// @param device 
	void Api::AddDevice(Device* device)
	{
		_devices.push_back(device);
		InputManager::GetInstance()->AddDevice(device);
	}

	/// @brief 
	/// @param device 
	void Api::RemoveDevice(Device* device)
	{
		std::erase(_devices, device);
		InputManager::GetInstance()->RemoveDevice(device);
	}

	/// @brief 
	void Api::Update()
	{
#if defined(PLATFORM_WINDOWS) || defined(PLATFORM_LINUX)
/*
		DesktopApplication* desktopApplication = DesktopApplication::GetInstance();
		if (desktopApplication != nullptr)
		{
			bool focused = desktopApplication->GetIsFocused();

			if (_previouslyFocused != focused)
			{
				_previouslyFocused = focused;
				if (focused == false)
				{
					for (Device* device : _vDevices)
					{
						if (device->GetType() != Device::Type::PAD || InputManager::GetInstance()->GetPadMappingIgnoreFocus() == false)
						{
							device->ReleaseAllInputs();
						}
					}
				}
			}
		}
*/
#endif
		UpdateDeviceValues();
	}

	/// @brief 
	/// @param deviceUid 
	/// @return 
	bool Api::HasDevice(const UID& deviceUid) const
	{
		for (const Device* device : _devices)
		{
			if (deviceUid == device->GetUid())
			{
				return true;
			}
		}

		return false;
	}

	/// @brief 
	/// @param deviceUid 
	/// @return 
	const Device* Api::GetDevice(const UID& deviceUid) const
	{
		for (const Device* device : _devices)
		{
			if (deviceUid == device->GetUid())
			{
				return device;
			}
		}

		return nullptr;
	}

	/// @brief 
	/// @return 
	const Vector<Device*> Api::GetDevices() const
	{
		return _devices;
	}

	/// @brief 
	/// @return 
	bool Api::IsInitialized() const
	{
		return _initialized;
	}

	/// @brief 
	/// @param initialized 
	void Api::SetInitialized(bool initialized)
	{
		_initialized = initialized;
	}

	/// @brief 
	/// @return 
	const String& Api::GetName() const
	{
		return _name;
	}
}

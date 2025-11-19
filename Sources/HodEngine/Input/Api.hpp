#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"
#include <cstdint>
#include <string_view>

namespace hod
{
	class UID;
}

namespace hod::input
{
	class Device;

	/// @brief
	class HOD_INPUT_API Api
	{
		friend class InputManager;

	public:
		enum DeviceUidOffset
		{
			RAW_INPUT = 1000,
			XINPUT = 2000,
			COUNT
		};

	public:
		Api(const std::string_view& name);
		Api(const Api&) = delete;
		Api(Api&&) = delete;
		virtual ~Api() = default;

		Api& operator=(const Api&) = delete;
		Api& operator=(Api&&) = delete;

		virtual bool Initialize() = 0;
		bool         IsInitialized() const;

		void Update();

		const String& GetName() const;

		bool                  HasDevice(const UID& deviceUid) const;
		const Device*         GetDevice(const UID& deviceUid) const;
		const Vector<Device*> GetDevices() const;

	protected:
		virtual void UpdateDeviceValues() = 0;

		void SetInitialized(bool initialized);

		void AddDevice(Device* device);
		void RemoveDevice(Device* device);

		static void SetDeviceConnected(Device* device, bool connected);
		static void NotifyDeviceConnected(Device* device);
		static void NotifyDeviceDisconnected(Device* device);

	private:
		String _name;

		bool            _initialized = false;
		Vector<Device*> _devices;
	};
}

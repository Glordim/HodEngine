#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string_view>
#include <string>
#include <vector>
#include <cstdint>

namespace hod
{
	class UID;
}

namespace hod::input
{
	class Device;

	/// @brief 
	class HOD_API Api
	{
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
		virtual							~Api() = default;

		Api&							operator = (const Api&) = delete;
		Api&							operator = (Api&&) = delete;

		virtual bool					Initialize() = 0;
		bool							IsInitialized() const;

		void							Update();

		const std::string&				GetName() const;

		bool							HasDevice(const UID& deviceUid) const;
		const Device*					GetDevice(const UID& deviceUid) const;
		const std::vector<Device*>		GetDevices() const;

	protected:

		virtual void					UpdateDeviceValues() = 0;

		void							SetInitialized(bool initialized);

		void							AddDevice(Device* device);

		static void						SetDeviceConnected(Device* device, bool connected);
		static void						NotifyDeviceConnected(Device* device);
		static void						NotifyDeviceDisconnected(Device* device);

	private:

		std::string						_name;

		bool							_initialized = false;
		std::vector<Device*>			_devices;
	};
}

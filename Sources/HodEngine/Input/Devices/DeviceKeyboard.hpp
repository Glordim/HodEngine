#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Device.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API DeviceKeyboard : public Device
	{
	public:

						DeviceKeyboard(const UID& uid, const std::string_view& name, Product product);
						DeviceKeyboard(const DeviceKeyboard&) = delete;
						DeviceKeyboard(DeviceKeyboard&&) = delete;
						~DeviceKeyboard() override = default;

		DeviceKeyboard&	operator = (const DeviceKeyboard&) = delete;
		DeviceKeyboard&	operator = (DeviceKeyboard&&) = delete;

	public:

		void				ClearBufferedTextIfNeeded();
		void				AppendCharactersToBufferedText(const std::string_view& characters);
		const String&	GetBufferedText();

	private:

		std::string			_bufferedText;
		bool				_readed = false;
	};
}

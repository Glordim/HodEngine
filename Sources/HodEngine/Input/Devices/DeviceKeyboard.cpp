#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Devices/DeviceKeyboard.hpp"

namespace hod::input
{
	/// @brief 
	/// @param uid 
	/// @param name 
	/// @param product 
	DeviceKeyboard::DeviceKeyboard(const UID& uid, const std::string_view& name, Product product)
	: Device(Type::KEYBOARD, uid, name, product)
	{

	}

	/// @brief 
	void DeviceKeyboard::ClearBufferedTextIfNeeded()
	{
		if (_readed == true)
		{
			_readed = false;
			_bufferedText.clear();
		}
	}

	/// @brief 
	/// @param characters 
	void DeviceKeyboard::AppendCharactersToBufferedText(const std::string_view& characters)
	{
		_bufferedText += characters;
	}

	/// @brief 
	/// @return 
	const std::string& DeviceKeyboard::GetBufferedText()
	{
		_readed = true;
		return _bufferedText;
	}
}

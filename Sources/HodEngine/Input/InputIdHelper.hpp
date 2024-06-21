#pragma once

#include "HodEngine/Input/InputId.hpp"

namespace hod::input
{
	/// @brief 
	class InputIdHelper
	{
	public:

		static const char*			GetName(InputId inputId, bool pretty = true);

		static InputId				PhysicalToVirtual(InputId inputId);
		static InputId				VirtualToPhysical(InputId inputId);

	private:

		static const char*			_inputsNamesMouse[InputId::MouseEndEnum - InputId::MouseStartEnum - 1];
		static const char*			_inputsNamesPad[InputId::PadEndEnum - InputId::PadStartEnum - 1];
		static const char*			_inputsNamesKeyboard[InputId::KeyboardEnumCount];
		static const char*			_inputsNamesKeyboardVirtual[InputId::KeyboardVirtualEnumCount];
		static const char*			_inputsNamesKeyboardVirtualPretty[InputId::KeyboardVirtualEnumCount];
	};
}

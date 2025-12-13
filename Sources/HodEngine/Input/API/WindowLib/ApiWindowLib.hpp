#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/WindowLib/KeyboardWindowLib.hpp"
#include "HodEngine/Input/API/WindowLib/MouseWindowLib.hpp"
#include "HodEngine/Input/InputId.hpp"

#include <HodEngine/Window/MouseButton.hpp>
#include <HodEngine/Window/ScanCode.hpp>

#include "HodEngine/Core/Event.hpp"

#include <mutex>

namespace hod::input
{
	/// @brief
	class HOD_INPUT_API ApiWindowLib : public Api
	{
	public:
		ApiWindowLib();
		ApiWindowLib(const ApiWindowLib&) = delete;
		ApiWindowLib(ApiWindowLib&&) = delete;
		~ApiWindowLib() override;

		ApiWindowLib& operator=(const ApiWindowLib&) = delete;
		ApiWindowLib& operator=(ApiWindowLib&&) = delete;

	public:
		bool Initialize() override;

	protected:
		void UpdateDeviceValues() override;

	private:
		MouseWindowLib    _mouse;
		KeyboardWindowLib _keyboard;
	};
}

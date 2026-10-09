#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/WindowLib/WindowLibKeyboard.hpp"
#include "HodEngine/Input/API/WindowLib/WindowLibMouse.hpp"
#include "HodEngine/Input/InputId.hpp"

#include <HodEngine/Window/MouseButton.hpp>
#include <HodEngine/Window/ScanCode.hpp>

#include "HodEngine/Core/Event.hpp"

#include <mutex>

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API WindowLibApi : public Api
	{
	public:
		WindowLibApi();
		WindowLibApi(const WindowLibApi&) = delete;
		WindowLibApi(WindowLibApi&&) = delete;
		~WindowLibApi() override;

		WindowLibApi& operator=(const WindowLibApi&) = delete;
		WindowLibApi& operator=(WindowLibApi&&) = delete;

	public:
		bool Initialize() override;

	protected:
		void UpdateDeviceValues() override;

	private:
		WindowLibMouse    _mouse;
		WindowLibKeyboard _keyboard;
	};
}

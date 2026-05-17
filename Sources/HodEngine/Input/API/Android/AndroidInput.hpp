#pragma once
#include "AndroidFakeMouse.hpp"
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/API/Android/AndroidFakeMouse.hpp"

namespace hod::inline input
{
	/// @brief
	class HOD_INPUT_API AndroidInput : public Api
	{
	public:
		AndroidInput();
		AndroidInput(const AndroidInput&) = delete;
		AndroidInput(AndroidInput&&) = delete;
		~AndroidInput() override;

		AndroidInput& operator=(const AndroidInput&) = delete;
		AndroidInput& operator=(AndroidInput&&) = delete;

	public:
		bool Initialize() override;

	protected:
		void UpdateDeviceValues() override;

	private:
		AndroidFakeMouse    _mouse;
	};
}

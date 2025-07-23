#pragma once
#include "HodEngine/Input/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

#include "HodEngine/Input/Input.hpp"

#include "HodEngine/Core/Vector.hpp"

namespace hod::input
{
	class Api;

	/// @brief 
	class HOD_INPUT_API InputManager
	{
		_Singleton(InputManager)

		friend class Api;

	public:

		bool							Initialize();
		void							UpdateJob();

		Input::State					GetInputState(InputId inputId) const;

		const Vector<Device*>			GetDevices() const;

	private:

										~InputManager();

		bool							InitializeApis();

		template<typename __API__>
		bool							CreateApi();
		bool							CreateApi(Api* api);

		void							AddDevice(Device* device);
		void							RemoveDevice(Device* device);

	private:

		Vector<Api*>					_apis;
		Vector<Device*>					_devices;

		MemberFunctionJob<InputManager>	_updateJob;
	};

	/// @brief 
	/// @tparam API_ 
	/// @return 
	template<typename API_>
	bool InputManager::CreateApi()
	{
		return CreateApi(DefaultAllocator::GetInstance().New<API_>());
	}
}

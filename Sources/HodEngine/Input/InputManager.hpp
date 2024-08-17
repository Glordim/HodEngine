#pragma once
#include "HodEngine/Input/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/Job/MemberFunctionJob.hpp>

#include "HodEngine/Input/Input.hpp"

#include <vector>

namespace hod::window
{
	class Window;
}

namespace hod::input
{
	class Api;

	/// @brief 
	class HOD_INPUT_API InputManager
	{
		_Singleton(InputManager)

	public:

		bool							Initialize(window::Window* window);
		void							UpdateJob();

		Input::State					GetInputState(InputId inputId) const;

	private:

										~InputManager();

		bool							InitializeApis(window::Window* window);

		template<typename __API__>
		bool							CreateApi(window::Window* window);
		bool							CreateApi(Api* api, window::Window* window);

	private:

		std::vector<Api*>				_apis;

		MemberFunctionJob<InputManager>	_updateJob;
	};

	/// @brief 
	/// @tparam API_ 
	/// @return 
	template<typename API_>
	bool InputManager::CreateApi(window::Window* window)
	{
		return CreateApi(new API_(), window);
	}
}

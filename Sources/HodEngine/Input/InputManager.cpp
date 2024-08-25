#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/InputManager.hpp"
#include "HodEngine/Input/Api.hpp"
#include "HodEngine/Input/Device.hpp"

#include "HodEngine/Core/Frame/FrameSequencer.hpp"
#include "HodEngine/Core/Job/JobScheduler.hpp"

namespace hod::input
{
	_SingletonConstructor(InputManager)
	: _updateJob(this, &InputManager::UpdateJob, JobQueue::FramedHighPriority, false)
	{

	}

	/// @brief 
	InputManager::~InputManager()
	{
		FrameSequencer::GetInstance()->RemoveJob(&_updateJob, FrameSequencer::Step::PreLogic);
		if (_updateJob.Cancel() == true)
		{
			_updateJob.Wait();
		}

		for (Api* api : _apis)
		{
			delete api;
		}
	}

	/// @brief 
	/// @return 
	bool InputManager::Initialize(window::Window* window)
	{
		if (InitializeApis(window) == false)
		{
			return false;
		}

		FrameSequencer::GetInstance()->InsertJob(&_updateJob, FrameSequencer::Step::PreLogic);
		return true;
	}

	/// @brief 
	/// @param api 
	/// @return 
	bool InputManager::CreateApi(Api* api, window::Window* window)
	{
		if (api->Initialize(window) == false)
		{
			delete api;
			return false;
		}
		_apis.push_back(api);
		return true;
	}

	/// @brief 
	void InputManager::UpdateJob()
	{
		/*
		JobScheduler::GetInstance()->Push(&_apiJobBatch);
		_apiJobBatch.Wait();
		*/

		for (Api* api : _apis)
		{
			api->Update();
		}
	}

	/// @brief 
	/// @param inputId 
	/// @return 
	Input::State InputManager::GetInputState(InputId inputId) const
	{
		Input::State maxState;
		maxState._inputId = inputId;
		maxState._flags = Input::Flag::Released;

		for (Api* api : _apis)
		{
			for (Device* device : api->GetDevices())
			{
				Input* input = device->GetInput(inputId);
				if (input != nullptr)
				{
					maxState.Merge(input->GetState());
				}
			}
		}

		return maxState;
	}
}

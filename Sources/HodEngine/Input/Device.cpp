#include "HodEngine/Input/Pch.hpp"
#include "HodEngine/Input/Device.hpp"
#include "HodEngine/Input/Input.hpp"
#include "HodEngine/Input/InputManager.hpp"

namespace hod::input
{
	/// @brief 
	/// @param type 
	/// @param uid 
	/// @param name 
	/// @param product 
	Device::Device(Type type, const UID& uid, const std::string_view& name, Product product, uint32_t stateSize)
		: _uid(uid)
		, _type(type)
		, _product(product)
		, _connected(false)
		, _stateSize(stateSize)
	{
		SetName(name);

		_previousState = DefaultAllocator::GetInstance().Allocate<State>(_stateSize);
		_currentState = DefaultAllocator::GetInstance().Allocate<State>(_stateSize);
		_nextState = DefaultAllocator::GetInstance().Allocate<State>(_stateSize);

		std::memset(_nextState, 0, _stateSize);
		std::memset(_currentState, 0, _stateSize);
		std::memset(_previousState, 0, _stateSize);
	}

	Device::~Device()
	{
		DefaultAllocator::GetInstance().Free(_previousState);
		DefaultAllocator::GetInstance().Free(_currentState);
		DefaultAllocator::GetInstance().Free(_nextState);
	}

	/// @brief 
	/// @param name 
	void Device::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief 
	/// @param inputId 
	/// @return 
	Input* Device::GetInput(InputId inputId) const
	{
		(void)inputId;
		/*
		for (Input* input : _inputs)
		{
			if (input->GetInputId() == inputId)
			{
				return input;
			}
		}
			*/
		return nullptr;
	}

	/// @brief 
	/// @return 
	const Vector<Input*>& Device::GetInputs() const
	{
		return _inputs;
	}

	/// @brief 
	/// @param input 
	void Device::AddInput(Input* input)
	{
		input->SetStatePtr(_currentState, _previousState);
		_inputs.push_back(input);
	}

	/// @brief 
	/// @return 
	Device::Type Device::GetType() const
	{
		return _type;
	}

	/// @brief 
	/// @return 
	const UID& Device::GetUid() const
	{
		return _uid;
	}

	/// @brief 
	/// @return 
	Device::Product Device::GetProduct() const
	{
		return _product;
	}

	/// @brief 
	/// @param eProduct 
	void Device::SetProduct(Product product)
	{
		_product = product;
	}

	/// @brief 
	/// @return 
	const String& Device::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @return 
	bool Device::IsConnected() const
	{
		return _connected;
	}

	/// @brief 
	/// @param connected 
	void Device::SetConnected(bool connected)
	{
		if (IsConnected() != connected)
		{
			_connected = connected;
		}
	}

	uint32_t Device::GetStateSize() const
	{
		return _stateSize;
	}

	const State* Device::GetPreviousState() const
	{
		return _previousState;
	}

	const State* Device::GetCurrentState() const
	{
		return _currentState;
	}

	State* Device::EditNextState()
	{
		return _nextState;
	}

	void Device::UpdateState()
	{
		std::memcpy(_previousState, _currentState, _stateSize);
		_stateLock.Lock();
		std::memcpy(_currentState, _nextState, _stateSize);
		ResetNextState();
		_stateLock.Unlock();
	}
}

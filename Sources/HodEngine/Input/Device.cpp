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
	Device::Device(Type type, const UID& uid, const std::string_view& name, Product product)
		: _uid(uid)
		, _type(type)
		, _product(product)
		, _connected(false)
	{
		SetName(name);
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
		for (Input* input : _inputs)
		{
			if (input->GetInputId() == inputId)
			{
				return input;
			}
		}
		return nullptr;
	}

	/// @brief 
	/// @return 
	const std::vector<Input*>& Device::GetInputs() const
	{
		return _inputs;
	}

	/// @brief 
	/// @param input 
	void Device::AddInput(Input* input)
	{
		_inputs.push_back(input);
		SetInputValue(*input, 0.0f);
	}

	/// @brief 
	void Device::ClearInputFlags()
	{
		for (Input* input : _inputs)
		{
			input->ClearInputFlags();
		}
	}

	/// @brief 
	void Device::UpdateInputFlags()
	{
		for (Input* input : _inputs)
		{
			input->UpdateInputFlags();
		}
	}

	/// @brief 
	void Device::ReleaseAllInputs()
	{
		for (Input* input : _inputs)
		{
			if (input->GetValue() != 0.0f) // Check if not already release to don't override JustRelease flag
			{
				input->SetValue(0.0f);
			}
		}
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
	const std::string& Device::GetName() const
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

	/// @brief 
	/// @param input 
	/// @param value 
	void Device::SetInputValue(Input& input, float value)
	{
		input.SetValue(value);
	}
}

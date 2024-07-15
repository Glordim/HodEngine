#include "HodEngine/ImGui/Window/WindowDescription.hpp" 

namespace hod::imgui
{
	std::vector<WindowDescription*>	WindowDescription::_descriptions;

	/// @brief 
	/// @param name 
	/// @param unique 
	/// @param allocateFunction 
	WindowDescription::WindowDescription(const char* name, bool unique, std::function<Window*()> allocateFunction)
	: _name(name)
	, _unique(unique)
	, _allocateFunction(allocateFunction)
	{
		_descriptions.push_back(this);
	}

	/// @brief 
	/// @return 
	Window* WindowDescription::InstantiateWindow()
	{
		return _allocateFunction();
	}

	/// @brief 
	/// @return 
	const char* WindowDescription::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @return 
	bool WindowDescription::IsUnique() const
	{
		return _unique;
	}

	/// @brief 
	/// @param name 
	/// @return 
	WindowDescription* WindowDescription::GetDescription(const char* name)
	{
		for (WindowDescription* description : _descriptions)
		{
			if (std::strcmp(description->GetName(), name) == 0)
			{
				return description;
			}
		}

		return nullptr;
	}
}

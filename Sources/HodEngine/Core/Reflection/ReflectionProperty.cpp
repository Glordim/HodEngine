#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/CharHelper.hpp"

#include <algorithm>

namespace hod
{
	/// @brief 
	/// @param offset 
	/// @param name 
	ReflectionProperty::ReflectionProperty(uint32_t offset, const char* name)
	: _offset(offset)
	, _name(name)
	{
#if defined(HOD_EDITOR)
		_displayName = GenerateDisplayName(_name);
#endif
	}

#if defined(HOD_EDITOR)
	std::string ReflectionProperty::GenerateDisplayName(const std::string_view& name)
	{
		std::string displayName;
		displayName.reserve(name.size());

		bool nextWord = true;
		for (char c : name)
		{
			if (c == '_')
			{
				if (displayName.empty() == false)
				{
					displayName += ' ';
					nextWord = true;
				}
				continue;
			}
			else if (nextWord == false && IsUpper(c) == true)
			{
				displayName += ' ';
				displayName += c;
			}
			else
			{
				if (nextWord == true && IsLower(c) == true)
				{
					c = ToUpper(c);
				}
				nextWord = false;
				displayName += c;
			}
		}

		return displayName;
	}

	const std::string& ReflectionProperty::GetDisplayName() const
	{
		return _displayName;
	}
#endif

	///@brief 
	///@return const std::vector<ReflectionTrait>& 
	const std::vector<ReflectionTrait*>& ReflectionProperty::GetTraits() const
	{
		return _traits;
	}

	///@brief 
	///@param trait 
	void ReflectionProperty::AddTrait(ReflectionTrait* trait)
	{
		_traits.push_back(trait);
	}

	/// @brief 
	/// @param metaType 
	/// @return 
	ReflectionTrait* ReflectionProperty::FindTrait(MetaType metaType) const
	{
		for (uint32_t index = 0; index < _traits.size(); ++index)
		{
			if (_traits[index]->GetMetaType() == metaType)
			{
				return _traits[index];
			}
		}
		return nullptr;
	}

	/// @brief 
	/// @return 
	uint32_t ReflectionProperty::GetOffset() const
	{
		return _offset;
	}

	/// @brief 
	/// @return 
	const char* ReflectionProperty::GetName() const
	{
		return _name;
	}
}

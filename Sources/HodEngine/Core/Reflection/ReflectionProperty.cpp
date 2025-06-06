#include "HodEngine/Core/Pch.hpp"
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
	}

	/// @brief 
	ReflectionProperty::~ReflectionProperty()
	{
		for (ReflectionTrait* trait : _traits)
		{
			DefaultAllocator::GetInstance().Delete(trait);
		}
	}
	

	/// @brief 
	/// @return 
	const String& ReflectionProperty::GetDisplayName() const
	{
		if (_displayName.empty())
		{
			_displayName = GenerateDisplayName(_name);
		}
		return _displayName;
	}

	///@brief 
	///@return const Vector<ReflectionTrait>& 
	const Vector<ReflectionTrait*>& ReflectionProperty::GetTraits() const
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

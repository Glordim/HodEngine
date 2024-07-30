#include "HodEngine/Core/pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"

#include <cassert>

namespace hod
{
	///@brief Construct a new ReflectionDescriptor::ReflectionDescriptor object
	///@param typeName 
	ReflectionDescriptor::ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent)
	: _typeName(typeName)
	, _parent(parent)
	{

	}

	/// @brief 
	/// @param data 
	ReflectionDescriptor::ReflectionDescriptor(const Data& data)
	: _typeName(data._name.data()) // todo
	, _parent(data._parent)
	, _allocateFunction(data._allocateFunction)
	, _sharedAllocateFunction(data._sharedAllocateFunction)
	, _compareFunction(data._compareFunction)
	, _metaType(data._metaType)
	{

	}

	/// @brief 
	/// @return 
	void* ReflectionDescriptor::CreateInstance() const
	{
		return _allocateFunction();
	}

	/// @brief 
	/// @return 
	std::shared_ptr<void> ReflectionDescriptor::CreateSharedInstance() const
	{
		return _sharedAllocateFunction();
	}

	/// @brief 
	/// @return 
	MetaType ReflectionDescriptor::GetMetaType() const
	{
		return _metaType;
	}

	/// @brief 
	/// @return 
	const std::string& ReflectionDescriptor::GetTypeName() const
	{
		return _typeName;
	}

	///@brief 
	///@return const std::vector<ReflectionTrait*>& 
	const std::vector<ReflectionTrait*>& ReflectionDescriptor::GetTraits() const
	{
		return _traits;
	}

	///@brief 
	///@return const std::vector<ReflectionProperty*>& 
	const std::vector<ReflectionProperty*>& ReflectionDescriptor::GetProperties() const
	{
		return _properties;
	}

	///@brief 
	///@param trait 
	void ReflectionDescriptor::AddTrait(ReflectionTrait* trait)
	{
		_traits.push_back(trait);
	}

	/// @brief 
	/// @param metaType 
	/// @return 
	ReflectionTrait* ReflectionDescriptor::FindTrait(MetaType metaType) const
	{
		for (uint32_t index = 0; index < _traits.size(); ++index)
		{
			if (_traits[index]->GetMetaType() == metaType)
			{
				return _traits[index];
			}
		}

		if (_fallbackTraitOnParent)
		{
			ReflectionDescriptor* parent = GetParent();
			if (parent != nullptr)
			{
				return parent->FindTrait(metaType);
			}
		}

		return nullptr;
	}

	/// @brief 
	/// @param fallbackOnParent 
	void ReflectionDescriptor::SetFallbackTraitOnParent(bool fallbackTraitOnParent)
	{
		_fallbackTraitOnParent = fallbackTraitOnParent;
	}

	///@brief 
	///@param property 
	void ReflectionDescriptor::AddProperty(ReflectionProperty* property)
	{
		_properties.push_back(property);
	}

	/// @brief 
	/// @param name 
	/// @return 
	ReflectionProperty* ReflectionDescriptor::FindProperty(const std::string_view& name, bool fallbackOnParent) const
	{
		for (ReflectionProperty* property : _properties)
		{
			if (property->GetName() == name)
			{
				return property;
			}
		}
		if (fallbackOnParent == true && _parent != nullptr)
		{
			return _parent->FindProperty(name, true);
		}
		return nullptr;
	}

	/// @brief 
	/// @return 
	ReflectionDescriptor* ReflectionDescriptor::GetParent() const
	{
		return _parent;
	}

	/// @brief 
	/// @param sourceInstance 
	/// @param destinationInstance 
	void ReflectionDescriptor::Copy(const void* sourceInstance, void* destinationInstance)
	{
        if (_parent != nullptr)
        {
            _parent->Copy(sourceInstance, destinationInstance);
        }

		for (ReflectionProperty* reflectionProperty : _properties)
		{
			reflectionProperty->Copy(sourceInstance, destinationInstance);
		}
	}

	/// @brief 
	/// @param left 
	/// @param right 
	/// @return 
	bool ReflectionDescriptor::Compare(const void* left, const void* right) const
	{
		assert(_compareFunction != nullptr);
		return _compareFunction(left, right);
	}
}

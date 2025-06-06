#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/CharHelper.hpp"

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
	, _deleteFunction(data._deleteFunction)
	, _sharedAllocateFunction(data._sharedAllocateFunction)
	, _compareFunction(data._compareFunction)
	, _metaType(data._metaType)
	{
	}

	/// @brief 
	/// @param data 
	void ReflectionDescriptor::Init(const Data& data)
	{
		_typeName = data._name.data(); // todo
		_parent = data._parent;
		_allocateFunction = data._allocateFunction;
		_deleteFunction = data._deleteFunction;
		_sharedAllocateFunction = data._sharedAllocateFunction;
		_compareFunction = data._compareFunction;
		_metaType = data._metaType;
	}

	/// @brief 
	/// @return 
	const String& ReflectionDescriptor::GetDisplayName() const
	{
		if (_displayName.empty())
		{
			_displayName = GenerateDisplayName(_typeName);
		}
		return _displayName;
	}

	/// @brief 
	ReflectionDescriptor::~ReflectionDescriptor()
	{
		for (ReflectionProperty* property : _properties)
		{
			DefaultAllocator::GetInstance().Delete(property);
		}

		for (ReflectionTrait* trait : _traits)
		{
			DefaultAllocator::GetInstance().Delete(trait);
		}
	}

	/// @brief 
	/// @return 
	void* ReflectionDescriptor::CreateInstance() const
	{
		return _allocateFunction();
	}

	/// @brief 
	/// @param instance 
	void ReflectionDescriptor::DeleteInstance(void* instance)
	{
		_deleteFunction(instance);
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
	const String& ReflectionDescriptor::GetTypeName() const
	{
		return _typeName;
	}

	///@brief 
	///@return const Vector<ReflectionTrait*>& 
	const Vector<ReflectionTrait*>& ReflectionDescriptor::GetTraits() const
	{
		return _traits;
	}

	///@brief 
	///@return const Vector<ReflectionProperty*>& 
	const Vector<ReflectionProperty*>& ReflectionDescriptor::GetProperties() const
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
	ReflectionTrait* ReflectionDescriptor::FindTrait(MetaType metaType, bool fallbackOnParent) const
	{
		for (uint32_t index = 0; index < _traits.size(); ++index)
		{
			if (_traits[index]->GetMetaType() == metaType)
			{
				return _traits[index];
			}
		}

		if (fallbackOnParent)
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
	/// @param metaType 
	void ReflectionDescriptor::RemoveTrait(MetaType metaType)
	{
		for (uint32_t index = 0; index < _traits.size(); ++index)
		{
			if (_traits[index]->GetMetaType() == metaType)
			{
				ReflectionTrait* trait = _traits[index];
				_traits.erase(_traits.begin() + index);
				DefaultAllocator::GetInstance().Delete(trait);
				return;
			}
		}
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
	/// @param descriptor 
	/// @return 
	bool ReflectionDescriptor::IsCompatible(const ReflectionDescriptor& descriptor) const
	{
		if (this == &descriptor)
		{
			return true;
		}

		if (_parent != nullptr)
		{
			return _parent->IsCompatible(descriptor);
		}

		return false;
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

	/// @brief 
	/// @param reflectionDescriptor 
	/// @param collectedInstances 
	/// @param instance 
	void ReflectionDescriptor::CollectObjectProperties(const ReflectionDescriptor& reflectionDescriptor, Vector<void*>& collectedInstances, void* instance)
	{
		for (ReflectionProperty* property : GetProperties())
		{
			if (property->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
			{
				ReflectionPropertyObject* objectProperty = static_cast<ReflectionPropertyObject*>(property);
				if (objectProperty->GetReflectionDescriptor()->IsCompatible(reflectionDescriptor))
				{
					void* objectInstance = objectProperty->GetValue(instance);
					if (objectInstance != nullptr)
					{
						collectedInstances.push_back(objectInstance);
					}
				}
			}
		}
	}
}

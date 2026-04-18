#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"

#include "HodEngine/Core/CharHelper.hpp"
#include "HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp"
#include "HodEngine/Core/Reflection/ReflectionProperty.hpp"
#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

#include <cassert>
#include <cstdint>

namespace hod
{
	///@brief Construct a new ReflectionDescriptor::ReflectionDescriptor object
	///@param typeName
	ReflectionDescriptor::ReflectionDescriptor(const char* typeName, ReflectionDescriptor* parent)
	: _typeName(typeName)
	, _type(hod::Hash::CompilationTimeFnv64(_typeName))
	, _parent(parent)
	{
	}

	/// @brief
	/// @param data
	ReflectionDescriptor::ReflectionDescriptor(const Data& data)
	: _typeName(data._name.data()) // todo
	, _type(hod::Hash::CompilationTimeFnv64(_typeName))
	, _parent(data._parent)
	, _allocateFunction(data._allocateFunction)
	, _deleteFunction(data._deleteFunction)
	, _compareFunction(data._compareFunction)
	{
	}

	/// @brief
	/// @param data
	void ReflectionDescriptor::Init(const Data& data)
	{
		_typeName = data._name.data(); // todo
		_type = hod::Hash::CompilationTimeFnv64(_typeName);
		_parent = data._parent;
		_allocateFunction = data._allocateFunction;
		_deleteFunction = data._deleteFunction;
		_compareFunction = data._compareFunction;
	}

	/// @brief
	/// @return
	const String& ReflectionDescriptor::GetDisplayName() const
	{
		if (_displayName.Empty())
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

	uint64_t ReflectionDescriptor::GetType() const
	{
		return _type;
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
	/// @param RttiType
	/// @return
	ReflectionTrait* ReflectionDescriptor::FindTrait(RttiType rttiType, bool fallbackOnParent) const
	{
		for (uint32_t index = 0; index < _traits.Size(); ++index)
		{
			if (_traits[index]->GetRttiType() == rttiType)
			{
				return _traits[index];
			}
		}

		if (fallbackOnParent)
		{
			ReflectionDescriptor* parent = GetParent();
			if (parent != nullptr)
			{
				return parent->FindTrait(rttiType);
			}
		}

		return nullptr;
	}

	/// @brief
	/// @param RttiType
	void ReflectionDescriptor::RemoveTrait(RttiType rttiType)
	{
		for (uint32_t index = 0; index < _traits.Size(); ++index)
		{
			if (_traits[index]->GetRttiType() == rttiType)
			{
				ReflectionTrait* trait = _traits[index];
				_traits.Erase(index);
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
		if (_type == descriptor._type)
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
			if (property->GetRttiType() == ReflectionPropertyObject::GetRttiTypeStatic())
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

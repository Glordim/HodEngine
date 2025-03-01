#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/EditorReflectedObject.hpp"
#include "HodEngine/Editor/EditorReflectedProperty.hpp"

#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>
#include <HodEngine/Core/Reflection/Properties/ReflectionPropertyObject.hpp>

#include <HodEngine/Game/Component.hpp>
#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/PrefabUtility.hpp>

namespace hod::editor
{
	EditorReflectedObject::EditorReflectedObject(void* instance, const ReflectionDescriptor* reflectionDescriptor, void* source)
	: _sourceInstance(source)
	, _instances({ instance })
	, _reflectionDescriptor(reflectionDescriptor)
	{
		GeneratePropertiesFromReflectionDescriptor(_reflectionDescriptor);
	}

	EditorReflectedObject::EditorReflectedObject(const std::vector<void*>& instances, const ReflectionDescriptor* reflectionDescriptor, void* source)
	: _sourceInstance(source)
	, _instances(instances)
	, _reflectionDescriptor(reflectionDescriptor)
	{
		GeneratePropertiesFromReflectionDescriptor(_reflectionDescriptor);
	}

	EditorReflectedObject::EditorReflectedObject(EditorReflectedProperty& sourceProperty)
	: _sourceProperty(&sourceProperty)
	{
		_instances.reserve(sourceProperty.GetInstances().size());

		ReflectionProperty* property = sourceProperty.GetReflectionProperty();
		if (property->GetMetaType() == ReflectionPropertyObject::GetMetaTypeStatic())
		{
			ReflectionPropertyObject* propertyObject = static_cast<ReflectionPropertyObject*>(property);
			_reflectionDescriptor = propertyObject->GetReflectionDescriptor();

			if (sourceProperty.GetSourceInstance() != nullptr)
			{
				_sourceInstance = propertyObject->GetInstance(sourceProperty.GetSourceInstance());
			}
			for (void* instance : sourceProperty.GetInstances())
			{
				_instances.push_back(propertyObject->GetInstance(instance));
			}
		}
		else if (property->GetMetaType() == ReflectionPropertyArray::GetMetaTypeStatic())
		{
			ReflectionPropertyArray* propertyArray = static_cast<ReflectionPropertyArray*>(property);
			_reflectionDescriptor = propertyArray->GetElementReflectionDescriptor();

			if (sourceProperty.GetSourceInstance() != nullptr)
			{
				_sourceInstance = propertyArray->GetValue<void*>(sourceProperty.GetSourceInstance(), sourceProperty.GetInternalIndex());
			}
			for (void* instance : sourceProperty.GetInstances())
			{
				_instances.push_back(propertyArray->GetValue<void*>(instance, sourceProperty.GetInternalIndex()));
			}
		}
		else
		{
			assert(false);
			return;
		}

		GeneratePropertiesFromReflectionDescriptor(_reflectionDescriptor);
	}

	EditorReflectedObject::~EditorReflectedObject()
	{
		for (auto* property : _properties)
		{
			delete property; // avoid alloc ?
		}
	}

	bool EditorReflectedObject::IsOverride() const
	{
		if (_sourceInstance == nullptr)
		{
			return false;
		}

		return (_reflectionDescriptor->Compare(_instances[0], _sourceInstance) == false);
	}

	void* EditorReflectedObject::GetInstance() const
	{
		return _instances[0];
	}

	const std::vector<void*>& EditorReflectedObject::GetInstances() const
	{
		return _instances;
	}

	EditorReflectedProperty* EditorReflectedObject::GetSourceProperty() const
	{
		return _sourceProperty;
	}

	void EditorReflectedObject::GeneratePropertiesFromReflectionDescriptor(const ReflectionDescriptor* reflectionDescriptor)
	{
		ReflectionDescriptor* parentReflectionDescriptor = reflectionDescriptor->GetParent();
		if (parentReflectionDescriptor != nullptr)
		{
			GeneratePropertiesFromReflectionDescriptor(parentReflectionDescriptor);
		}

		for (ReflectionProperty* reflectionProperty : reflectionDescriptor->GetProperties())
		{
			EditorReflectedProperty* property = new EditorReflectedProperty(_instances, _sourceInstance, reflectionProperty, this);
			_properties.push_back(property);
		}
	}

	std::vector<EditorReflectedProperty*>& EditorReflectedObject::GetProperties()
	{
		return _properties;
	}
}

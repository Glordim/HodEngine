#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <vector>
#include <memory>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::game
{
	class Component;
}

namespace hod::editor
{
	class EditorReflectedProperty;

	class HOD_EDITOR_API EditorReflectedObject
	{
	public:

		EditorReflectedObject(void* instance, const ReflectionDescriptor* reflectionDescriptor, void* source);
		EditorReflectedObject(const std::vector<void*>& instances, const ReflectionDescriptor* reflectionDescriptor, void* source);

		EditorReflectedObject(EditorReflectedProperty& sourceProperty);
		~EditorReflectedObject();

	public:

		bool IsEditingMultipleInstance() const;
		bool IsOverride() const;

		std::vector<EditorReflectedProperty*>&  GetProperties();

		void*                       GetInstance() const;
		const std::vector<void*>&   GetInstances() const;

		EditorReflectedProperty*    GetSourceProperty() const;

	private:

		void GeneratePropertiesFromReflectionDescriptor(const ReflectionDescriptor* reflectionDescriptor);

	private:

		void*                   _sourceInstance = nullptr;
		std::vector<void*>      _instances;
		const ReflectionDescriptor*   _reflectionDescriptor = nullptr;
		EditorReflectedProperty*    _sourceProperty = nullptr;

		std::vector<EditorReflectedProperty*>    _properties;
	};
}
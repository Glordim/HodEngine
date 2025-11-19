#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/Vector.hpp"
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
	class EditorTabWindow;
	class EditorReflectedProperty;

	class HOD_EDITOR_API EditorReflectedObject
	{
	public:

		EditorReflectedObject(void* instance, const ReflectionDescriptor* reflectionDescriptor, void* source, EditorTabWindow* editorTabWindow);
		EditorReflectedObject(const Vector<void*>& instances, const ReflectionDescriptor* reflectionDescriptor, void* source, EditorTabWindow* editorTabWindow);

		EditorReflectedObject(EditorReflectedProperty& sourceProperty);
		~EditorReflectedObject();

	public:

		bool IsEditingMultipleInstance() const;
		bool IsOverride() const;

		EditorReflectedProperty*			FindProperty(std::string_view name) const;
		Vector<EditorReflectedProperty*>&  GetProperties();

		void*                       GetInstance() const;
		const Vector<void*>&   		GetInstances() const;

		EditorReflectedProperty*    GetSourceProperty() const;
		EditorTabWindow*			GetEditorTabWindow() const;

	private:

		void GeneratePropertiesFromReflectionDescriptor(const ReflectionDescriptor* reflectionDescriptor);

	private:

		void*                   _sourceInstance = nullptr;
		Vector<void*>      _instances;
		const ReflectionDescriptor*   _reflectionDescriptor = nullptr;
		EditorReflectedProperty*    _sourceProperty = nullptr;
		EditorTabWindow*		_editorTabWindow = nullptr;

		Vector<EditorReflectedProperty*>    _properties;
	};
}
#pragma once

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::editor
{
	///@brief 
	class ComponentCustomEditor
	{
	public:

									ComponentCustomEditor() = default;
									ComponentCustomEditor(const ComponentCustomEditor&) = delete;
									ComponentCustomEditor(ComponentCustomEditor&&) = delete;
			virtual					~ComponentCustomEditor() = default;

			ComponentCustomEditor	operator = (const ComponentCustomEditor&) = delete;
			ComponentCustomEditor	operator = (ComponentCustomEditor&&) = delete;

	public:

		virtual bool				OnDrawInspector(void* object, ReflectionDescriptor* reflectionDescriptor);
		virtual bool				OnDrawGizmo(void* object, ReflectionDescriptor* reflectionDescriptor);
	};
}

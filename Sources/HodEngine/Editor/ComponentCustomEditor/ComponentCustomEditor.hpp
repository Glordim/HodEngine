#pragma once
#include <HodEngine/HodEngine.hpp>

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::editor
{
	///@brief 
	class HOD_API ComponentCustomEditor
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

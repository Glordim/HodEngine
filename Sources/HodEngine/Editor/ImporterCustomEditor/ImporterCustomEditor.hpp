#pragma once

namespace hod
{
	class ReflectionDescriptor;
}

namespace hod::editor
{	
	///@brief 
	class ImporterCustomEditor
	{
	public:

									ImporterCustomEditor() = default;
									ImporterCustomEditor(const ImporterCustomEditor&) = delete;
									ImporterCustomEditor(ImporterCustomEditor&&) = delete;
			virtual					~ImporterCustomEditor() = default;

			ImporterCustomEditor	operator = (const ImporterCustomEditor&) = delete;
			ImporterCustomEditor	operator = (ImporterCustomEditor&&) = delete;

	public:

		virtual bool				OnDrawInspector(void* object, ReflectionDescriptor* reflectionDescriptor);
	};
}

#pragma once

#if defined(HOD_EDITOR)

namespace hod
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

		virtual void				OnDrawInspector();
		virtual void				OnDrawGizmo();
	};
}

#endif

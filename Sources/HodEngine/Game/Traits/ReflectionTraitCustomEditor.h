#pragma once

#if defined(HOD_EDITOR)

#include "HodEngine/Core/Reflection/ReflectionTrait.h"

namespace hod
{
	class ComponentCustomEditor;

	///@brief 
	class ReflectionTraitCustomEditor : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitDisplayName, ReflectionTrait)

	public:

										ReflectionTraitCustomEditor(ComponentCustomEditor* customEditor);
										ReflectionTraitCustomEditor(const ReflectionTraitCustomEditor& copy) = default;
										ReflectionTraitCustomEditor(ReflectionTraitCustomEditor&& move) = default;
										~ReflectionTraitCustomEditor() override;

		ReflectionTraitCustomEditor&	operator = (const ReflectionTraitCustomEditor& copy) = default;
		ReflectionTraitCustomEditor&	operator = (ReflectionTraitCustomEditor&& move) = default;

	public:

		ComponentCustomEditor*			GetCustomEditor() const;

	private:

		ComponentCustomEditor*			_customEditor = nullptr;
	};
}

#endif
#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionTrait.hpp"

namespace hod::editor
{
	class ComponentCustomEditor;

	///@brief 
	class HOD_EDITOR_API ReflectionTraitComponentCustomEditor : public ReflectionTrait
	{
		META_TYPE(ReflectionTraitComponentCustomEditor, ReflectionTrait)

	public:

												ReflectionTraitComponentCustomEditor(ComponentCustomEditor* customEditor);
												ReflectionTraitComponentCustomEditor(const ReflectionTraitComponentCustomEditor& copy) = default;
												ReflectionTraitComponentCustomEditor(ReflectionTraitComponentCustomEditor&& move) = default;
												~ReflectionTraitComponentCustomEditor() override;

		ReflectionTraitComponentCustomEditor&	operator = (const ReflectionTraitComponentCustomEditor& copy) = default;
		ReflectionTraitComponentCustomEditor&	operator = (ReflectionTraitComponentCustomEditor&& move) = default;

	public:

		ComponentCustomEditor*					GetCustomEditor() const;

	private:

		ComponentCustomEditor*					_customEditor = nullptr;
	};
}

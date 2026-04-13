#pragma once
#include "HodEngine/GameEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	/// @brief 
	class HOD_GAME_EDITOR_API WeakComponentCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:

									WeakComponentCustomPropertyDrawer() = default;
									WeakComponentCustomPropertyDrawer(const WeakComponentCustomPropertyDrawer&) = delete;
									WeakComponentCustomPropertyDrawer(WeakComponentCustomPropertyDrawer&&) = delete;
									~WeakComponentCustomPropertyDrawer() override = default;

		WeakComponentCustomPropertyDrawer&	operator = (const WeakComponentCustomPropertyDrawer&) = delete;
		WeakComponentCustomPropertyDrawer&	operator = (WeakComponentCustomPropertyDrawer&&) = delete;

	public:

		bool						Draw(EditorReflectedProperty& editorReflectedProperty) override;
	};
}

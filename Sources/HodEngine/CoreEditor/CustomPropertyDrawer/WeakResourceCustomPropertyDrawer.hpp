#pragma once
#include "HodEngine/CoreEditor/Export.hpp"

#include "HodEngine/Editor/CustomPropertyDrawer/CustomPropertyDrawer.hpp"

namespace hod::inline core
{
	class ReflectionPropertyObject;
}
namespace hod::inline gamesystems
{
	class WeakResourceBase;
}

namespace hod::editor
{
	/// @brief
	class HOD_CORE_EDITOR_API WeakResourceCustomPropertyDrawer : public CustomPropertyDrawer
	{
	public:
		WeakResourceCustomPropertyDrawer() = default;
		WeakResourceCustomPropertyDrawer(const WeakResourceCustomPropertyDrawer&) = delete;
		WeakResourceCustomPropertyDrawer(WeakResourceCustomPropertyDrawer&&) = delete;
		~WeakResourceCustomPropertyDrawer() override = default;

		WeakResourceCustomPropertyDrawer& operator=(const WeakResourceCustomPropertyDrawer&) = delete;
		WeakResourceCustomPropertyDrawer& operator=(WeakResourceCustomPropertyDrawer&&) = delete;

	public:
		bool Draw(EditorReflectedProperty& editorReflectedProperty) override;

		static bool Draw(WeakResourceBase& weakResource);
	};
}

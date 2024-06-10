#pragma once

#include "HodEngine/Editor/PropertyCustomEditor/CustomPropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	/// @brief 
	class WeakResourceCustomEditor : public CustomPropertyDrawer
	{
	public:

									WeakResourceCustomEditor() = default;
									WeakResourceCustomEditor(const WeakResourceCustomEditor&) = delete;
									WeakResourceCustomEditor(WeakResourceCustomEditor&&) = delete;
									~WeakResourceCustomEditor() override = default;

		WeakResourceCustomEditor&	operator = (const WeakResourceCustomEditor&) = delete;
		WeakResourceCustomEditor&	operator = (WeakResourceCustomEditor&&) = delete;

	public:

		bool						Draw(EditorReflectedObject& reflectedObject) override;
	};
}

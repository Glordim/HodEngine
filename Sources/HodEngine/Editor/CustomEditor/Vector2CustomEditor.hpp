#pragma once

#if defined(HOD_EDITOR)

#include "HodEngine/Game/Traits/PropertyDrawer.hpp"

namespace hod
{
	class ReflectionPropertyObject;
}

namespace hod::editor
{
	/// @brief 
	class Vector2CustomEditor : public game::PropertyDrawer
	{
	public:

								Vector2CustomEditor() = default;
								Vector2CustomEditor(const Vector2CustomEditor&) = delete;
								Vector2CustomEditor(Vector2CustomEditor&&) = delete;
								~Vector2CustomEditor() override = default;

		Vector2CustomEditor		operator = (const Vector2CustomEditor&) = delete;
		Vector2CustomEditor		operator = (Vector2CustomEditor&&) = delete;

	public:

		bool					Draw(void* instance, ReflectionPropertyObject* property) override;
	};
}

#endif

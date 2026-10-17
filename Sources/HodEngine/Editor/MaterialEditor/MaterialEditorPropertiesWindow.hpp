#pragma once
#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Editor/Export.hpp"
#include "HodEngine/Editor/EditorTabWindow.hpp"

#include <HodEngine/ImGui/Window/Window.hpp>

namespace hod::inline core
{
	class ReflectionDescriptor;
}

namespace hod::inline math
{
	class Vector4;
}

namespace hod::inline editor
{
	class EditorReflectedObject;

	/// @brief 
	class HOD_EDITOR_API MaterialEditorPropertiesWindow : public EditorTabWindow
	{
		REFLECTED_CLASS(MaterialEditorPropertiesWindow, EditorTabWindow)

	public:

					MaterialEditorPropertiesWindow() = default;
					MaterialEditorPropertiesWindow(EditorTab* editorTab);
					~MaterialEditorPropertiesWindow() override = default;

	public:

		void		DrawContent() override;

		// A Float4 parameter, as a color one if it is marked [Color] in the shader. Shared with the material instance editor.
		static bool	DrawVector4(Vector4& value, bool color);
	};
}

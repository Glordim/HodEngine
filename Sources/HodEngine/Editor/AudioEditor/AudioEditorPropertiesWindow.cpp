#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/AudioEditor/AudioEditorPropertiesWindow.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/ImGui/ImGuiManager.hpp>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>
#include <HodEngine/ImGui/Helper.hpp>

#include <HodEngine/Core/Serialization/Serializer.hpp>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(AudioEditorPropertiesWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	/// @param editorTab 
	AudioEditorPropertiesWindow::AudioEditorPropertiesWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{

	}

	/// @brief 
	void AudioEditorPropertiesWindow::DrawContent()
	{
	}
}

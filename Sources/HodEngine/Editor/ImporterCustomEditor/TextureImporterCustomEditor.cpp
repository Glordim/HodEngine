#include "HodEngine/Editor/ImporterCustomEditor/TextureImporterCustomEditor.hpp"
#include "HodEngine/Editor/Importer/TextureImporter.hpp"

#include "HodEngine/ImGui/DearImGui/imgui.h"

namespace hod::editor
{
	/// @brief 
	bool TextureImporterCustomEditor::OnDrawInspector(void* object, ReflectionDescriptor* reflectionDescriptor)
	{
		bool changed = false;

		TextureImporterSettings* textureImporterSettings = static_cast<TextureImporterSettings*>(object);

		ImGui::TextUnformatted("Texture");
		ImGui::Spacing();
		changed |= ImGui::Checkbox("Generate mipmap", &textureImporterSettings->_generateMipmap);
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::TextUnformatted("Sprite");
		ImGui::Spacing();

		return false;
	}
}

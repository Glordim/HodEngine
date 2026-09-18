#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/UIPrefabEditor/UIPrefabEditorViewportSettings.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportSettings::Guide, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Guide::_name, "Name");
		AddPropertyT(reflectionDescriptor, &Guide::_resolution, "Resolution");
		AddPropertyT(reflectionDescriptor, &Guide::_color, "Color");
		AddPropertyT(reflectionDescriptor, &Guide::_enabled, "Enabled");
	}

	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportSettings, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &UIPrefabEditorViewportSettings::_guides, "Guides");
		AddPropertyT(reflectionDescriptor, &UIPrefabEditorViewportSettings::_guidesVisible, "GuidesVisible");
		AddPropertyT(reflectionDescriptor, &UIPrefabEditorViewportSettings::_gridVisible, "GridVisible");
		AddPropertyT(reflectionDescriptor, &UIPrefabEditorViewportSettings::_gridCellSize, "GridCellSize");
	}

	namespace
	{
		Path GetSettingsFilePath()
		{
			Path path = FileSystem::GetUserSettingsPath();
			path /= ("HodEngine");
			path /= ("UIPrefabEditorViewport.json");
			return path;
		}
	}

	void UIPrefabEditorViewportSettings::Load()
	{
		Path path = GetSettingsFilePath();
		if (FileSystem::GetInstance()->Exists(path) == false)
		{
			return;
		}

		Document           document;
		DocumentReaderJson jsonReader;
		if (jsonReader.Read(document, path) == true)
		{
			Serializer::Deserialize(*this, document.GetRootNode());
		}
	}

	bool UIPrefabEditorViewportSettings::Save()
	{
		Path path = GetSettingsFilePath();

		Document document;
		if (Serializer::Serialize(*this, document.GetRootNode()) == false)
		{
			return false;
		}

		FileSystem::GetInstance()->CreateDirectories(path.ParentPath());
		DocumentWriterJson jsonWriter;
		return jsonWriter.Write(document, path);
	}
}

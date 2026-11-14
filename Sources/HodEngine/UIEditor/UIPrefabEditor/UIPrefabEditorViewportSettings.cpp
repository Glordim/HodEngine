#include "HodEngine/UIEditor/Pch.hpp"
#include "HodEngine/UIEditor/UIPrefabEditor/UIPrefabEditorViewportSettings.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportSettings::Device, reflectionDescriptor)
	{
		HOD_PROPERTY(_name);
		HOD_PROPERTY(_resolution);
	}

	DESCRIBE_REFLECTED_CLASS(UIPrefabEditorViewportSettings, reflectionDescriptor)
	{
		HOD_PROPERTY(_devices);
		HOD_PROPERTY(_activeDevice);
		HOD_PROPERTY(_deviceRotated);
		HOD_PROPERTY(_gridVisible);
		HOD_PROPERTY(_gridCellSize);
		HOD_PROPERTY(_preview);
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
			ResetDevices();
			return;
		}

		Document           document;
		DocumentReaderJson jsonReader;
		if (jsonReader.Read(document, path) == true)
		{
			Serializer::Deserialize(*this, document.GetRootNode());
		}

		// Saved before devices existed (they replaced the guides): start from the built-in list.
		if (document.GetRootNode().GetChild("_devices") == nullptr)
		{
			ResetDevices();
		}

		if (_activeDevice >= (int32_t)_devices.Size())
		{
			_activeDevice = -1;
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

	void UIPrefabEditorViewportSettings::ResetDevices()
	{
		// Landscape resolutions: rotating the preview gives the portrait ones.
		_devices.Clear();
		_devices.PushBack(Device {"Full HD 16:9", Vector2(1920.0f, 1080.0f)});
		_devices.PushBack(Device {"Desktop 16:10", Vector2(1920.0f, 1200.0f)});
		_devices.PushBack(Device {"Ultrawide 21:9", Vector2(2560.0f, 1080.0f)});
		_devices.PushBack(Device {"Tablet 4:3", Vector2(2048.0f, 1536.0f)});
		_devices.PushBack(Device {"Phone 19.5:9", Vector2(2556.0f, 1179.0f)});
		_devices.PushBack(Device {"Phone 20:9", Vector2(2400.0f, 1080.0f)});

		_activeDevice = -1;
		_deviceRotated = false;
	}

	Vector2 UIPrefabEditorViewportSettings::GetPreviewResolution() const
	{
		if (_activeDevice < 0 || _activeDevice >= (int32_t)_devices.Size())
		{
			return Vector2::Zero;
		}

		const Vector2& resolution = _devices[(uint32_t)_activeDevice]._resolution;
		return _deviceRotated ? Vector2(resolution.GetY(), resolution.GetX()) : resolution;
	}
}

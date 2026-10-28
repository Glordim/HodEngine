#include "HodEngine/GameSystems/Pch.hpp"
#include "HodEngine/GameSystems/ProjectSettings/ProjectSettingsManager.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include "HodEngine/Core/Document/DocumentWriterJson.hpp"
#include "HodEngine/Core/FileSystem/FileSystem.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/Reflection/ReflectionDescriptor.hpp"
#include "HodEngine/Core/Serialization/Serializer.hpp"

namespace hod::inline gamesystems
{
	_SingletonConstructor(ProjectSettingsManager) {}

	ProjectSettingsManager::~ProjectSettingsManager()
	{
		for (ProjectSettings* settings : _settings)
		{
			settings->GetReflectionDescriptorV().DeleteInstance(settings);
		}
	}

	/// @brief Creates the instance of the class descriptor describes (with its default values, overwritten by its file if already loaded)
	/// @return the instance, the existing one if already registered
	ProjectSettings* ProjectSettingsManager::Register(const ReflectionDescriptor& descriptor)
	{
		ProjectSettings* settings = Get(descriptor);
		if (settings != nullptr)
		{
			return settings;
		}

		settings = static_cast<ProjectSettings*>(descriptor.CreateInstance());
		_settings.PushBack(settings);

		if (_directory.Empty() == false)
		{
			LoadSettings(*settings);
		}
		return settings;
	}

	/// @brief
	/// @param descriptor
	void ProjectSettingsManager::Unregister(const ReflectionDescriptor& descriptor)
	{
		for (uint32_t index = 0; index < _settings.Size(); ++index)
		{
			if (_settings[index]->GetReflectionDescriptorV().GetType() == descriptor.GetType())
			{
				descriptor.DeleteInstance(_settings[index]);
				_settings.Erase(index);
				return;
			}
		}
	}

	/// @brief
	/// @param descriptor
	/// @return
	ProjectSettings* ProjectSettingsManager::Get(const ReflectionDescriptor& descriptor) const
	{
		for (ProjectSettings* settings : _settings)
		{
			if (settings->GetReflectionDescriptorV().GetType() == descriptor.GetType())
			{
				return settings;
			}
		}
		return nullptr;
	}

	/// @brief
	/// @return
	const Vector<ProjectSettings*>& ProjectSettingsManager::GetAll() const
	{
		return _settings;
	}

	/// @brief
	/// @param directory
	/// @return false if a settings file couldn't be read (the others are loaded anyway)
	bool ProjectSettingsManager::Load(const Path& directory)
	{
		_directory = directory;

		bool result = true;
		for (ProjectSettings* settings : _settings)
		{
			result &= LoadSettings(*settings);
		}
		return result;
	}

	/// @brief
	/// @param settings
	/// @return
	bool ProjectSettingsManager::Save(const ProjectSettings& settings) const
	{
		if (_directory.Empty())
		{
			OUTPUT_ERROR("ProjectSettingsManager::Save: no directory loaded");
			return false;
		}

		if (FileSystem::GetInstance()->Exists(_directory) == false && FileSystem::GetInstance()->CreateDirectories(_directory) == false)
		{
			OUTPUT_ERROR("ProjectSettingsManager::Save: can't create {}", _directory);
			return false;
		}

		const ReflectionDescriptor& descriptor = settings.GetReflectionDescriptorV();

		Document document;
		if (Serializer::Serialize(descriptor, static_cast<const void*>(&settings), document.GetRootNode()) == false)
		{
			return false;
		}

		DocumentWriterJson writer;
		return writer.Write(document, GetFilePath(_directory, descriptor));
	}

	/// @brief
	/// @return
	const Path& ProjectSettingsManager::GetDirectory() const
	{
		return _directory;
	}

	/// @brief
	/// @param directory
	/// @param descriptor
	/// @return
	Path ProjectSettingsManager::GetFilePath(const Path& directory, const ReflectionDescriptor& descriptor)
	{
		return directory / (descriptor.GetTypeName() + ".json");
	}

	/// @brief
	/// @param settings
	/// @return true if loaded or without file
	bool ProjectSettingsManager::LoadSettings(ProjectSettings& settings) const
	{
		const ReflectionDescriptor& descriptor = settings.GetReflectionDescriptorV();

		Path path = GetFilePath(_directory, descriptor);
		if (FileSystem::GetInstance()->Exists(path) == false)
		{
			return true;
		}

		Document           document;
		DocumentReaderJson reader;
		if (reader.Read(document, path) == false)
		{
			OUTPUT_ERROR("ProjectSettingsManager: can't read {}", path);
			return false;
		}

		return Serializer::Deserialize(descriptor, static_cast<void*>(&settings), document.GetRootNode());
	}
}

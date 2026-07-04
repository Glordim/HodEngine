#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/Cooker/Cooker.hpp"

#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Core/Output/OutputService.hpp"
#include "HodEngine/Core/ScopedGuard.hpp"
#include "HodEngine/Core/UID.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"
#include "HodEngine/Editor/Editor.hpp"
#include "HodEngine/Editor/Project.hpp"
#include "HodEngine/Editor/TaskTracker/TaskTracker.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"

#include "HodEngine/Core/Hash.hpp"

#include <type_traits>
#include <utility>

namespace hod::inline editor
{
	namespace
	{
		struct PlatformEntry
		{
			Platform         value;
			std::string_view name;
		};

		constexpr PlatformEntry platformEntries[] = {
			{Platform::Windows, "Windows"}, {Platform::MacOs, "MacOs"}, {Platform::Linux, "Linux"}, {Platform::Android, "Android"}, {Platform::Ios, "Ios"},
		};

		struct ConfigEntry
		{
			Config           value;
			std::string_view name;
		};

		constexpr ConfigEntry configEntries[] = {
			{Config::Debug, "Debug"},
			{Config::Retail, "Retail"},
		};

		struct LanguageVariant
		{
			uint32_t         value;
			std::string_view name;
		};

		constexpr size_t languageEntryCount = sizeof(ResourceVariant::LanguageEntries) / sizeof(ResourceVariant::LanguageEntries[0]);
	}

	std::string_view ToString(Config config)
	{
		for (const ConfigEntry& configEntry : configEntries)
		{
			if (configEntry.value == config)
			{
				return configEntry.name;
			}
		}
		return "";
	}

	void Cooker::SetAssetType(std::string_view assetType)
	{
		_assetType = Hash::ComputeXxh3_64(assetType);
	}

	void Cooker::SetCookerVersion(uint32_t cookerVersion)
	{
		_cookerVersion = cookerVersion;
	}

	void Cooker::ClearDataBlocks()
	{
		for (DataBlock* dataBlock : _dataBlocks)
		{
			DefaultAllocator::GetInstance().Delete(dataBlock);
		}
		_dataBlocks.Clear();
	}

	bool Cooker::Cook(const Path& path, uint32_t platforms, uint8_t configs, uint32_t languages, uint64_t taskId)
	{
		AssetDatabase::FileSystemMapping* fileSystemNode = AssetDatabase::GetInstance()->FindFileSystemMappingFromPath(path);
		if (fileSystemNode != nullptr && fileSystemNode->_asset != nullptr)
		{
			return Cook(*fileSystemNode->_asset, platforms, configs, languages, taskId);
		}
		else
		{
			OUTPUT_ERROR("Cooker::Cook: Unable to retrieve asset at path {}", path);
			return false;
		}
	}

	bool Cooker::Cook(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages, uint64_t taskId)
	{
		_taskId = taskId;
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 0.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Opening asset");

		FileStream assetStream;
		if (assetStream.Open(asset.GetPath()) == false)
		{
			OUTPUT_ERROR("Cooker::Cook: Unable to open asset file '{}'", asset.GetPath());
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}
		assetStream.Close();

		ClearDataBlocks();
		ScopedGuard cleanupDataBlocks = [&]() { ClearDataBlocks(); };

		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 2.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Creating tmp folder");
		_tmpDir = FileSystem::GetTemporaryPath() / "HodEngine" / Project::GetInstance()->GetName() / "Cook" / asset.GetUid().ToString(); // todo add datetime
		ScopedGuard cleanupTmp = [&]() { FileSystem::GetInstance()->RemoveAll(_tmpDir); };
		if (FileSystem::GetInstance()->CreateDirectories(_tmpDir) == false)
		{
			OUTPUT_ERROR("Importer::Import: Unable to create tmp dir '{}'", _tmpDir);
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 5.0f);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Processing asset");
		if (FillDataBlock(asset, platforms, configs, languages) == false)
		{
			Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
			return false;
		}

		for (DataBlock* dataBlock : _dataBlocks)
		{
			dataBlock->GetStream().Flush();
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, "Writing resource");
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 80.0f);

		for (const PlatformEntry& platformEntry : platformEntries)
		{
			uint32_t platformBit = std::to_underlying(platformEntry.value);
			if ((platforms & platformBit) == 0)
			{
				continue;
			}

			for (const ConfigEntry& configEntry : configEntries)
			{
				uint8_t configBit = std::to_underlying(configEntry.value);
				if ((configs & configBit) == 0)
				{
					continue;
				}

				// A resource whose data never varies per requested language (every relevant data block either
				// doesn't apply or fully covers the requested language mask) is written once, under
				// "Unlocalized", instead of once per language.
				bool languageInvariant = (languages == std::to_underlying(ResourceVariant::Language::All));
				if (languageInvariant)
				{
					for (DataBlock* dataBlock : _dataBlocks)
					{
						if ((dataBlock->GetPlatforms() & platformBit) == 0 || (dataBlock->GetConfigs() & configBit) == 0)
						{
							continue;
						}
						uint32_t overlap = dataBlock->GetLanguages() & languages;
						if (overlap != 0 && overlap != languages)
						{
							languageInvariant = false;
							break;
						}
					}
				}

				LanguageVariant languageVariants[languageEntryCount + 1];
				size_t languageVariantCount = 0;
				if (languageInvariant)
				{
					languageVariants[languageVariantCount++] = {languages, ResourceVariant::UnlocalizedName};
				}
				else
				{
					for (const ResourceVariant::LanguageEntry& languageEntry : ResourceVariant::LanguageEntries)
					{
						uint32_t languageBit = std::to_underlying(languageEntry.value);
						if ((languages & languageBit) != 0)
						{
							languageVariants[languageVariantCount++] = {languageBit, languageEntry.name};
						}
					}
				}

				for (size_t languageIndex = 0; languageIndex < languageVariantCount; ++languageIndex)
				{
					uint32_t         languageBit = languageVariants[languageIndex].value;
					std::string_view languageName = languageVariants[languageIndex].name;

					ResourceContainer resourceContainer;
					resourceContainer.SetType(_assetType);
					resourceContainer.SetCookerVersion(_cookerVersion);
					resourceContainer.SetAssetContentHash(asset.GetContentHash());

					bool hasDataBlock = false;
					for (DataBlock* dataBlock : _dataBlocks)
					{
						if ((dataBlock->GetPlatforms() & platformBit) != 0 && (dataBlock->GetConfigs() & configBit) != 0 && (dataBlock->GetLanguages() & languageBit) != 0)
						{
							resourceContainer.SetDataBlock(dataBlock->GetName(), dataBlock->GetStream(), dataBlock->GetCompressed());
							hasDataBlock = true;
						}
					}

					if (hasDataBlock == false)
					{
						continue;
					}

					Path destinationDir = Project::GetInstance()->GetResourceDirPath() / Path(platformEntry.name) / Path(configEntry.name) / Path(languageName);
					if (FileSystem::GetInstance()->CreateDirectories(destinationDir) == false)
					{
						OUTPUT_ERROR("Cooker::Cook: Unable to create resource dir '{}'", destinationDir);
						Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
						return false;
					}

					Path destinationPath = destinationDir / (asset.GetUid().ToString() + ".res").CStr();
					if (resourceContainer.Save(destinationPath, _tmpDir) == false)
					{
						Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Failed);
						return false;
					}
				}
			}
		}

		Editor::GetInstance()->GetTaskTracker().UpdateTaskStatus(taskId, TaskStatus::Succeeded);
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 100.0f);
		return true;
	}

	Stream& Cooker::AddDataBlockStream(std::string_view name, bool compressed, uint32_t platforms, uint8_t configs, uint32_t languages)
	{
		for (DataBlock* dataBlock : _dataBlocks)
		{
			if (dataBlock->GetName() == name && dataBlock->GetPlatforms() == platforms && dataBlock->GetConfigs() == configs && dataBlock->GetLanguages() == languages)
			{
				return dataBlock->GetStream();
			}
		}
		Path tmpFile = _tmpDir / (String(name) + "_" + String::Format("%u", _dataBlocks.Size()));
		_dataBlocks.PushBack(DefaultAllocator::GetInstance().New<DataBlock>(name, tmpFile, compressed, platforms, configs, languages));
		return _dataBlocks.Back()->GetStream();
	}

	void Cooker::UpdateFillDataBlockProgress(float percent)
	{
		Editor::GetInstance()->GetTaskTracker().UpdateTaskProgress(_taskId, 5.0f + 75.0f * percent);
	}

	void Cooker::UpdateFillDataBlockDescription(std::string_view description)
	{
		Editor::GetInstance()->GetTaskTracker().UpdateTaskDescription(_taskId, description);
	}
}

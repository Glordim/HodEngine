#pragma once

#include <stdint.h>

#include <filesystem>
#include <map>

#include <HodEngine/Core/Src/UID.h>
#include <HodEngine/Core/Src/Event.h>

#include <HodEngine/Core/Src/Job/MemberFunctionJob.h>

namespace hod::editor
{
	class Asset;
	class Project;

	/// @brief 
	class AssetDatabase
	{
	public:
								AssetDatabase();
								~AssetDatabase();

		bool					Init();

	private:

		void					OnProjectLoaded(Project* project);
		void					OnProjectClosed(Project* project);

		void					ExploreAndDetectAsset(const std::filesystem::path dir);
		void					FilesystemWatcherJob();

	private:

		struct FileSystem
		{
			std::filesystem::path	_path;
			Asset*					_asset;

			std::vector<FileSystemMapping>	_children;
		};

		std::map<UID, Asset*>			_uidToAssetMap;
		std::vector<FileSystemMapping>	_filesystem;

		Event<Project*>::Slot	_onProjectLoadedSlot;
		Event<Project*>::Slot	_onProjectClosedSlot;

		void*								_filesystemWatcherHandle;
		MemberFunctionJob<AssetDatabase>	_filesystemWatcherJob;
	};
}

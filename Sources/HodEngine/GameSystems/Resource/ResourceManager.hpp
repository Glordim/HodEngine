#pragma once
#include "HodEngine/GameSystems/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/UID.hpp"
#include "HodEngine/GameSystems/Resource/ResourceContainer.hpp"
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <map>

#include "HodEngine/Core/Singleton.hpp"

#include <memory>
#include <functional>

#undef FindResource

namespace hod::inline gamesystems
{
	class Resource;

	class HOD_GAME_SYSTEMS_API ResourceManager : public Singleton<ResourceManager>
	{
		friend class Singleton<ResourceManager>;
		friend class hod::Allocator;

	public:
		void SetResourceDirectory(const Path& directory);
		void SetFileNotFoundCallback(const std::function<bool(const UID&)>& callback) { _fileNotFoundCallback = callback; }
		void SetCheckUpToDateCallback(std::function<bool(const UID&, const ResourceContainer&)> callback) { _checkUpToDateCallback = callback; }

		std::shared_ptr<Resource> GetResource(const ReflectionDescriptor& reflectionDescriptor, const UID& uid);

		template<typename _Resource_>
		std::shared_ptr<_Resource_> GetResource(const UID& uid);

	protected:
		ResourceManager() = default;
		~ResourceManager() override = default;

	private:
		std::shared_ptr<Resource> FindResource(const UID& uid) const;
		bool                      Load(Resource* resource, const UID& uid);

	private:
		Path _directory;

		std::map<UID, std::weak_ptr<Resource>> _resources;

		std::function<bool(const UID&)> _fileNotFoundCallback = nullptr;
		std::function<bool(const UID&, const ResourceContainer&)> _checkUpToDateCallback = nullptr;
	};
}

#include "ResourceManager.inl"

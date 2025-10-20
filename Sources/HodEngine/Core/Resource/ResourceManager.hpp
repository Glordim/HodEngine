#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/UID.hpp"
#include <HodEngine/Core/FileSystem/Path.hpp>
#include <map>

#include "HodEngine/Core/Singleton.hpp"

#undef FindResource

namespace hod
{
	class Resource;

	class HOD_CORE_API ResourceManager : public Singleton<ResourceManager>
	{
		friend class Singleton<ResourceManager>;
		friend class Allocator;

	public:
		void SetResourceDirectory(const Path& directory);

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
	};
}

#include "ResourceManager.inl"

#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"
#include "HodEngine/Core/UID.hpp"
#include <istream>
#include <filesystem>
#include <map>

#include "HodEngine/Core/Singleton.hpp"

#undef FindResource

namespace hod
{
	class Resource;

	class HOD_CORE_API ResourceManager : public Singleton<ResourceManager>
	{
		friend class Singleton<ResourceManager>;

	public:

		void	SetResourceDirectory(const std::filesystem::path& directory);

		std::shared_ptr<Resource> 	GetResource(ReflectionDescriptor* reflectionDescriptor, const UID& uid);

		template<typename _Resource_>
		std::shared_ptr<_Resource_> GetResource(const UID& uid);

	protected:

			ResourceManager() = default;
			~ResourceManager() override = default;

	private:

		std::shared_ptr<Resource>	FindResource(const UID& uid) const;
		bool						Load(Resource* resource, const UID& uid);

	private:

		std::filesystem::path _directory;

		std::map<UID, std::weak_ptr<Resource>>	_resources;
	};
}

#include "ResourceManager.inl"

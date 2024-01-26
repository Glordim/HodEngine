
namespace hod
{
	template<typename _Resource_>
	std::shared_ptr<_Resource_> ResourceManager::GetResource(const UID& uid)
	{
		std::shared_ptr<Resource> existingResource = FindResource(uid);
		if (existingResource != nullptr)
		{
			return std::static_pointer_cast<_Resource_>(existingResource);
		}

		std::shared_ptr<_Resource_> resource = std::make_shared<_Resource_>();
		if (Load(resource.get(), uid) == false)
		{
			return nullptr;
		}
		_resources[uid] = std::static_pointer_cast<Resource>(resource);
		return resource;
	}
}

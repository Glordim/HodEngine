
namespace hod
{
	/// @brief 
	/// @tparam _Resource_ 
	/// @param uid 
	/// @return 
	template<typename _Resource_>
	std::shared_ptr<_Resource_> ResourceManager::GetResource(const UID& uid)
	{
		return std::static_pointer_cast<_Resource_>(GetResource(_Resource_::GetReflectionDescriptor(), uid));
	}
}

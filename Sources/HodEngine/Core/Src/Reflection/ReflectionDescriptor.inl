
namespace hod
{
	namespace CORE
	{
		template<typename __TRAIT_TYPE__, typename... Args>
		void ReflectionDescriptor::AddTrait(Args&&... args)
		{
			AddTrait(new __TRAIT_TYPE__(std::forward<Args>(args)...));
		}

		template<typename __PROPERTY_TYPE__, typename... Args>
		void ReflectionDescriptor::AddProperty(Args&&... args)
		{
			AddProperty(new __PROPERTY_TYPE__(std::forward<Args>(args)...));
		}
	}
}

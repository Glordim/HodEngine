
namespace hod
{
	namespace CORE
	{
		template<typename __TRAIT_TYPE__, typename... Args>
		void ReflectionProperty::AddTrait(Args&&... args)
		{
			AddTrait(new __TRAIT_TYPE__(std::forward<Args>(args)...));
		}
	}
}

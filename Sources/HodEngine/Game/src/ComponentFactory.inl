
namespace hod
{
	namespace game
	{
		template<typename _Component_>
		bool ComponentFactory::Register()
		{
			static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

			core::ReflectionDescriptor* reflectionDescriptor = _Component_::GetReflectionDescriptor();
			// todo make a dedicated function to transform type name to pretty name
			core::ReflectionTraitDisplayName* reflectionTraitDisplayName = reflectionDescriptor->FindTrait<core::ReflectionTraitDisplayName>();
			if (reflectionTraitDisplayName == nullptr)
			{
				reflectionDescriptor->AddTrait<core::ReflectionTraitDisplayName>(reflectionDescriptor->GetTypeName());
			}
			//

			_metaTypeToDescriptors[_Component_::GetMetaTypeStatic()] = reflectionDescriptor;
			return true;
		}
	}
}

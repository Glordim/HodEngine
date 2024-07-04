
namespace hod::game
{
	template<typename _Component_>
	bool ComponentFactory::Register()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

		ReflectionDescriptor* reflectionDescriptor = _Component_::GetReflectionDescriptor();
		// todo make a dedicated function to transform type name to pretty name
		ReflectionTraitDisplayName* reflectionTraitDisplayName = reflectionDescriptor->FindTrait<ReflectionTraitDisplayName>();
		if (reflectionTraitDisplayName == nullptr)
		{
			reflectionDescriptor->AddTrait<ReflectionTraitDisplayName>(reflectionDescriptor->GetTypeName());
		}
		//

		_metaTypeToDescriptors[_Component_::GetMetaTypeStatic()] = reflectionDescriptor;
		return true;
	}

	template<typename _Component_>
	bool ComponentFactory::Unregister()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

		auto it = _metaTypeToDescriptors.find(_Component_::GetMetaTypeStatic());
		if (it != _metaTypeToDescriptors.end())
		{
			_metaTypeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

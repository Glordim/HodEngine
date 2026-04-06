
namespace hod::game
{
	template<typename _Component_>
	bool ComponentFactory::Register()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

		_typeToDescriptors[_Component_::GetReflectionDescriptor().GetType()] = &_Component_::GetReflectionDescriptor();
		return true;
	}

	template<typename _Component_>
	bool ComponentFactory::Unregister()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

		auto it = _typeToDescriptors.find(_Component_::GetReflectionDescriptor().GetType());
		if (it != _typeToDescriptors.end())
		{
			_typeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

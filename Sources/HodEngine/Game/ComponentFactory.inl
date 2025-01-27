
namespace hod::game
{
	template<typename _Component_>
	bool ComponentFactory::Register()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

		_metaTypeToDescriptors[_Component_::GetMetaTypeStatic()] = &_Component_::GetReflectionDescriptor();;
		return true;
	}

	template<typename _Component_>
	bool ComponentFactory::Unregister()
	{
		static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

		auto it = _metaTypeToDescriptors.find(&_Component_::GetMetaTypeStatic());
		if (it != _metaTypeToDescriptors.end())
		{
			_metaTypeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

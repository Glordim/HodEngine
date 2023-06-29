
namespace hod
{
	namespace game
	{
		template<typename _Component_>
		bool ComponentFactory::Register()
		{
			static_assert(std::is_base_of<Component, _Component_>::value, "_Component_ must derive from Component to use ComponentFactory::Register()");

			_metaTypeToDescriptors[_Component_::GetMetaTypeStatic()] = &const_cast<ComponentDescriptor&>(_Component_::GetDescriptorStatic());
			return true;
		}
	}
}

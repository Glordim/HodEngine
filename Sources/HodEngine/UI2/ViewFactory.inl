
namespace hod::inline ui2
{
	template<typename _View_>
	bool ViewFactory::Register()
	{
		static_assert(std::is_base_of<View, _View_>::value, "_View_ must derive from View to use ViewFactory::Register()");

		_typeToDescriptors[_View_::GetReflectionDescriptor().GetType()] = &_View_::GetReflectionDescriptor();
		return true;
	}

	template<typename _View_>
	bool ViewFactory::Unregister()
	{
		static_assert(std::is_base_of<View, _View_>::value, "_View_ must derive from View to use ViewFactory::Unregister()");

		auto it = _typeToDescriptors.find(_View_::GetReflectionDescriptor().GetType());
		if (it != _typeToDescriptors.end())
		{
			_typeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

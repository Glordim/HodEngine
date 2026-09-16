
namespace hod::inline ui2
{
	template<typename _Layout_>
	bool LayoutFactory::Register()
	{
		static_assert(std::is_base_of<Layout, _Layout_>::value, "_Layout_ must derive from Layout to use LayoutFactory::Register()");

		_typeToDescriptors[_Layout_::GetReflectionDescriptor().GetType()] = &_Layout_::GetReflectionDescriptor();
		return true;
	}

	template<typename _Layout_>
	bool LayoutFactory::Unregister()
	{
		static_assert(std::is_base_of<Layout, _Layout_>::value, "_Layout_ must derive from Layout to use LayoutFactory::Unregister()");

		auto it = _typeToDescriptors.find(_Layout_::GetReflectionDescriptor().GetType());
		if (it != _typeToDescriptors.end())
		{
			_typeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

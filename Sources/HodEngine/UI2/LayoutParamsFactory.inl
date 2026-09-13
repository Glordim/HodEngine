
namespace hod::inline ui2
{
	template<typename _LayoutParams_>
	bool LayoutParamsFactory::Register()
	{
		static_assert(std::is_base_of<LayoutParams, _LayoutParams_>::value, "_LayoutParams_ must derive from LayoutParams to use LayoutParamsFactory::Register()");

		_typeToDescriptors[_LayoutParams_::GetReflectionDescriptor().GetType()] = &_LayoutParams_::GetReflectionDescriptor();
		return true;
	}

	template<typename _LayoutParams_>
	bool LayoutParamsFactory::Unregister()
	{
		static_assert(std::is_base_of<LayoutParams, _LayoutParams_>::value, "_LayoutParams_ must derive from LayoutParams to use LayoutParamsFactory::Unregister()");

		auto it = _typeToDescriptors.find(_LayoutParams_::GetReflectionDescriptor().GetType());
		if (it != _typeToDescriptors.end())
		{
			_typeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

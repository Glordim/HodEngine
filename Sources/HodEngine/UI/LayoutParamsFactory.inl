#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	/// @return false (with an error) if registered already: the first one stays
	template<typename _LayoutParams_>
	bool LayoutParamsFactory::Register()
	{
		static_assert(std::is_base_of<LayoutParams, _LayoutParams_>::value, "_LayoutParams_ must derive from LayoutParams to use LayoutParamsFactory::Register()");

		ReflectionDescriptor& descriptor = _LayoutParams_::GetReflectionDescriptor();
		if (_typeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("LayoutParamsFactory: the layout params '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
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

#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	/// @return false (with an error) if registered already: the first one stays
	template<typename _Layout_>
	bool LayoutFactory::Register()
	{
		static_assert(std::is_base_of<Layout, _Layout_>::value, "_Layout_ must derive from Layout to use LayoutFactory::Register()");

		ReflectionDescriptor& descriptor = _Layout_::GetReflectionDescriptor();
		if (_typeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("LayoutFactory: the layout '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
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

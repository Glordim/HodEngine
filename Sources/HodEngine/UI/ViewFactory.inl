#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	/// @return false (with an error) if registered already: the first one stays
	template<typename _View_>
	bool ViewFactory::Register()
	{
		static_assert(std::is_base_of<View, _View_>::value, "_View_ must derive from View to use ViewFactory::Register()");

		ReflectionDescriptor& descriptor = _View_::GetReflectionDescriptor();
		if (_typeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("ViewFactory: the view '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
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

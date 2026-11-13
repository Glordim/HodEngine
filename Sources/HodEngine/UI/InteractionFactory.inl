#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline ui
{
	/// @return false (with an error) if registered already: the first one stays
	template<typename _Interaction_>
	bool InteractionFactory::Register()
	{
		static_assert(std::is_base_of<Interaction, _Interaction_>::value, "_Interaction_ must derive from Interaction to use InteractionFactory::Register()");

		ReflectionDescriptor& descriptor = _Interaction_::GetReflectionDescriptor();
		if (_typeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("InteractionFactory: the interaction '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
		return true;
	}

	template<typename _Interaction_>
	bool InteractionFactory::Unregister()
	{
		static_assert(std::is_base_of<Interaction, _Interaction_>::value, "_Interaction_ must derive from Interaction to use InteractionFactory::Unregister()");

		auto it = _typeToDescriptors.find(_Interaction_::GetReflectionDescriptor().GetType());
		if (it != _typeToDescriptors.end())
		{
			_typeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}


namespace hod::inline ui2
{
	template<typename _Interaction_>
	bool InteractionFactory::Register()
	{
		static_assert(std::is_base_of<Interaction, _Interaction_>::value, "_Interaction_ must derive from Interaction to use InteractionFactory::Register()");

		_typeToDescriptors[_Interaction_::GetReflectionDescriptor().GetType()] = &_Interaction_::GetReflectionDescriptor();
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

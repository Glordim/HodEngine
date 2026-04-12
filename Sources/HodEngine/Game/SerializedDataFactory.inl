
namespace hod::game
{
	template<typename _SerializedData_>
	bool SerializedDataFactory::Register()
	{
		static_assert(std::is_base_of<SerializedData, _SerializedData_>::value, "_SerializedData_ must derive from SerializedData to use SerializedDataFactory::Register()");

		_RttiTypeToDescriptors[_SerializedData_::GetReflectionDescriptor().GetType()] = &_SerializedData_::GetReflectionDescriptor();
		return true;
	}

	template<typename _SerializedData_>
	bool SerializedDataFactory::Unregister()
	{
		static_assert(std::is_base_of<SerializedData, _SerializedData_>::value, "_SerializedData_ must derive from SerializedData to use SerializedDataFactory::Register()");

		auto it = _RttiTypeToDescriptors.find(_SerializedData_::GetReflectionDescriptor().GetType());
		if (it != _RttiTypeToDescriptors.end())
		{
			_RttiTypeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

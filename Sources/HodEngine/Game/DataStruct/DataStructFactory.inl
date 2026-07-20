
namespace hod::inline game
{
	template<typename _DataStruct_>
	bool DataStructFactory::Register()
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use DataStructFactory::Register()");

		_RttiTypeToDescriptors[_DataStruct_::GetReflectionDescriptor().GetType()] = &_DataStruct_::GetReflectionDescriptor();
		return true;
	}

	template<typename _DataStruct_>
	bool DataStructFactory::Unregister()
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use DataStructFactory::Register()");

		auto it = _RttiTypeToDescriptors.find(_DataStruct_::GetReflectionDescriptor().GetType());
		if (it != _RttiTypeToDescriptors.end())
		{
			_RttiTypeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

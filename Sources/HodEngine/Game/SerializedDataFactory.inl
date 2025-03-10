
namespace hod::game
{
	template<typename _SerializedData_>
	bool SerializedDataFactory::Register()
	{
		static_assert(std::is_base_of<SerializedData, _SerializedData_>::value, "_SerializedData_ must derive from SerializedData to use SerializedDataFactory::Register()");

		_metaTypeToDescriptors[_SerializedData_::GetMetaTypeStatic()] = &_SerializedData_::GetReflectionDescriptor();
		return true;
	}

	template<typename _SerializedData_>
	bool SerializedDataFactory::Unregister()
	{
		static_assert(std::is_base_of<SerializedData, _SerializedData_>::value, "_SerializedData_ must derive from SerializedData to use SerializedDataFactory::Register()");

		auto it = _metaTypeToDescriptors.find(_SerializedData_::GetMetaTypeStatic());
		if (it != _metaTypeToDescriptors.end())
		{
			_metaTypeToDescriptors.erase(it);
			return true;
		}
		return false;
	}
}

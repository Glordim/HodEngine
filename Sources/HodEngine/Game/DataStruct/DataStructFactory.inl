#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Core/Reflection/ReflectionDescriptor.hpp>

namespace hod::inline game
{
	/// @return false (with an error) if registered already: the first one stays
	template<typename _DataStruct_>
	bool DataStructFactory::Register()
	{
		static_assert(std::is_base_of<DataStruct, _DataStruct_>::value, "_DataStruct_ must derive from DataStruct to use DataStructFactory::Register()");

		ReflectionDescriptor& descriptor = _DataStruct_::GetReflectionDescriptor();
		if (_RttiTypeToDescriptors.try_emplace(descriptor.GetType(), &descriptor).second == false)
		{
			OUTPUT_ERROR("DataStructFactory: the data struct '{}' is registered already", descriptor.GetDisplayName());
			return false;
		}
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

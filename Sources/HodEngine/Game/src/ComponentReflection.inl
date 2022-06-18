
namespace hod
{
	namespace game
	{
		template<typename __type__>
		bool ComponentReflection::Register()
		{
			_dumpFunctions.push_back([](rapidjson::Document::AllocatorType& allocator)
			{
				return __type__::GetReflectionDescriptor()->Serialize(allocator);
			});

			return true;
		}
	}
}

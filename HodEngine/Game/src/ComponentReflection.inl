
namespace HOD
{
	namespace GAME
	{
		template<typename __type__>
		bool ComponentReflection::Register()
		{
			_dumpFunctions.push_back([](rapidjson::Document& document)
			{
				__type__::GetReflectionDescriptor()->Serialize(document);
			});

			return true;
		}
	}
}

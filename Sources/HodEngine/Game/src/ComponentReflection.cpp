#include "ComponentReflection.h"

namespace hod
{
	template<>
	game::ComponentReflection* Singleton<game::ComponentReflection>::_instance = nullptr;

	namespace game
	{
		/// @brief 
		/// @param rootNode 
		/// @param allocator 
		/// @return 
		bool ComponentReflection::Dump(rapidjson::Value& rootNode, rapidjson::Document::AllocatorType& allocator)
		{
			rapidjson::Value array(rapidjson::kArrayType);

			for (const std::function<rapidjson::Value(rapidjson::Document::AllocatorType&)>& dumpFunction : _dumpFunctions)
			{
				rapidjson::Value descriptor = dumpFunction(allocator);

				array.PushBack(descriptor, allocator);
			}
			
			rootNode.AddMember("Components", array, allocator);
			return true;
		}
	}
}

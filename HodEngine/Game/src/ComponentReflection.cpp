#include "ComponentReflection.h"

namespace HOD
{
	template<>
	GAME::ComponentReflection* CORE::Singleton<GAME::ComponentReflection>::_instance = nullptr;

	namespace GAME
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

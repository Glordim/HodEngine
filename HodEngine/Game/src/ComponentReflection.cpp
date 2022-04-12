#include "ComponentReflection.h"

#include <rapidjson/document.h>

namespace HOD
{
	template<>
	GAME::ComponentReflection* CORE::Singleton<GAME::ComponentReflection>::_instance = nullptr;

	namespace GAME
	{
		///
		///@brief 
		///
		///@param dirPath 
		///@return true 
		///
		bool ComponentReflection::DumpToDir(const char* dirPath)
		{
			rapidjson::Document document;
			document.StartArray();

			for (const std::function<void(rapidjson::Document&)>& dumpFunction : _dumpFunctions)
			{
				dumpFunction(document);
			}

			document.End();

			return true;
		}
	}
}

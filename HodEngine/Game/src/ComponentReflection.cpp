#include "ComponentReflection.h"

#include <rapidjson/document.h>
#include <fstream>

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

			document.EndArray(_dumpFunctions.size());

			std::ofstream stream;
			stream.open(dirPath, std::ofstream::out);
			if (stream.is_open() == true)
			{
				stream.write(document.GetString(), document.GetStringLength());
				stream.close();
			}
			else
			{
				// todo error
			}

			return true;
		}
	}
}

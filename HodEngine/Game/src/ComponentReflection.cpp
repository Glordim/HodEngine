#include "ComponentReflection.h"

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
			return false;
		}
	}
}

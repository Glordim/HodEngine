#include "ActorReflection.h"

namespace HOD
{
	template<>
	GAME::ActorReflection* Singleton<GAME::ActorReflection>::_instance = nullptr;

	namespace GAME
	{
		///
		///@brief 
		///
		///@param dirPath 
		///@return true 
		///
		bool ActorReflection::DumpToDir(const char* dirPath)
		{
			return false;
		}
	}
}
#include "HodEngine/Game/src/ActorReflection.h"

namespace hod
{
	template<>
	game::ActorReflection* Singleton<game::ActorReflection>::_instance = nullptr;

	namespace game
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

#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Singleton.hpp>

#include <vector>

namespace hod
{
	namespace game
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API ActorReflection
		{
			_Singleton(ActorReflection)

		public:

			template<typename __type__>
			bool				Register();

			bool				DumpToDir(const char* dirPath);

		protected:
								~ActorReflection() = default;
		};
	}
}

#include "ActorReflection.inl"

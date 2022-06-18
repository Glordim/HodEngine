#pragma once

#include <HodEngine/Core/Src/Singleton.h>

#include <vector>

namespace hod
{
	namespace game
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class ActorReflection : public Singleton<ActorReflection>
		{
			friend class Singleton<ActorReflection>;

		public:

			template<typename __type__>
			bool				Register();

			bool				DumpToDir(const char* dirPath);

		protected:

								ActorReflection() = default;
								~ActorReflection() override = default;

		private:

		};
	}
}

#include "ActorReflection.inl"

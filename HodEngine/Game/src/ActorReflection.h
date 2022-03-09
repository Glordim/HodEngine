#pragma once

#include <Core/Src/Singleton.h>

#include <vector>

namespace HOD
{
	namespace GAME
	{
		class Scene;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class ActorReflection : public CORE::Singleton<ActorReflection>
		{
			friend class CORE::Singleton<ActorReflection>;

		public:

			template<typename __type__>
			bool				Register();

			bool				DumpToDir(const char* dirPath);

		protected:

								ActorReflection();
								~ActorReflection() override;

		private:

		};
	}
}

#include "ActorReflection.inl"

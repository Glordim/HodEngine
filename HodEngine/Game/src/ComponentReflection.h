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
		class ComponentReflection : public CORE::Singleton<ComponentReflection>
		{
			friend class CORE::Singleton<ComponentReflection>;

		public:

			template<typename __type__>
			bool				Register();

			bool				DumpToDir(const char* dirPath);

		protected:

								ComponentReflection();
								~ComponentReflection() override;

		private:

		};
	}
}

#include "ComponentReflection.inl"

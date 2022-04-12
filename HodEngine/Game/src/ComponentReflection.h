#pragma once

#include <Core/Src/Singleton.h>

#include <vector>
#include <functional>

#include <rapidjson/document.h>

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

			std::vector<std::function<void(rapidjson::Document&)>>	_dumpFunctions;
		};
	}
}

#include "ComponentReflection.inl"

#pragma once

#include <string>
#include <unordered_map>

#include <Core/Src/GenericManager.h>
#include <Core/Src/Singleton.h>

namespace HOD
{
	namespace RENDERER
	{
		class Material;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialManager : public CORE::GenericManager<Material>, public CORE::Singleton<CORE::GenericManager<Material>, MaterialManager>
		{
			friend class CORE::Singleton<CORE::GenericManager<Material>, MaterialManager>;

		public:

			CORE::UID									CreateMaterial(const std::string& shaderName, bool useDepth = true);

		private:
														MaterialManager();
														MaterialManager(const MaterialManager& copy) = delete;
			void										operator=(const MaterialManager& right) = delete;
			virtual										~MaterialManager();
		};
	}
}

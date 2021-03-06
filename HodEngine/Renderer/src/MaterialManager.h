#pragma once

#include <string>
#include <unordered_map>

#include <Core/Src/GenericManager.h>
#include <Core/Src/Singleton.h>

#include "RHI/Material.h"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialManager : public CORE::GenericManager<Material>, public CORE::Singleton<CORE::GenericManager<Material>, MaterialManager>
		{
			friend class CORE::Singleton<CORE::GenericManager<Material>, MaterialManager>;

		public:

			CORE::UID	CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true);

		private:
						MaterialManager();
						MaterialManager(const MaterialManager& copy) = delete;
			void		operator=(const MaterialManager& right) = delete;
			virtual		~MaterialManager();
		};
	}
}

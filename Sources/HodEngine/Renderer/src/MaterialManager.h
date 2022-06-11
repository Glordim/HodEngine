#pragma once

#include <string>
#include <unordered_map>

#include <HodEngine/Core/Src/GenericManager.h>
#include <HodEngine/Core/Src/Singleton.h>

#include "RHI/Material.h"

namespace HOD
{
	namespace RENDERER
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialManager : public CORE::GenericManager<Material>
		{
			_Singleton(MaterialManager)

				MaterialManager() = default;

		public:

			CORE::UID	CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true);
		};
	}
}

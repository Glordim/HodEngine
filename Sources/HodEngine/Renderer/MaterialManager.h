#pragma once

#include <string>
#include <unordered_map>

#include <HodEngine/Core/GenericManager.h>
#include <HodEngine/Core/Singleton.h>

#include "HodEngine/Renderer/RHI/Material.h"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialManager : public CORE::GenericManager<Material>
		{
			_Singleton(MaterialManager)

		public:

			UID	CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true);
		};
	}
}

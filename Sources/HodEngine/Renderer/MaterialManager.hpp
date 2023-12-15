#pragma once

#include <string>
#include <unordered_map>

#include <HodEngine/Core/GenericManager.hpp>
#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Renderer/RHI/Material.hpp"

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class MaterialManager : public GenericManager<Material>
		{
			_Singleton(MaterialManager)

		public:

			UID	CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true);
		};
	}
}

#pragma once
#include <HodEngine/HodEngine.hpp>

#include <string>
#include <unordered_map>

#include <HodEngine/Core/GenericManager.hpp>
#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Renderer/RHI/Material.hpp"

#include <utility>

namespace hod
{
	namespace renderer
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_API MaterialManager : public GenericManager<Material>
		{
			_Singleton(MaterialManager)

		public:

			enum class BuiltinMaterial : uint32_t
			{
				P2fT2f_Texture_Unlit,
				P2fC4f_Unlit_Fill_Triangle,
				P2fC4f_Unlit_Fill_TriangleFan,
				P2fC4f_Unlit_Line_TriangleFan,
				P2fC4f_Unlit_Line_Line,

				Count,
			};

		public:

			const Material*	GetBuiltinMaterial(BuiltinMaterial buildMaterial);

			UID	CreateMaterial(const std::string& shaderName, Material::PolygonMode polygonMode = Material::PolygonMode::Fill, Material::Topololy topololy = Material::Topololy::TRIANGLE, bool useDepth = true);

		private:

			std::array<Material*, static_cast<uint32_t>(BuiltinMaterial::Count)>	_builtinMaterials = { nullptr }; // c++23 std::to_underlying
		};
	}
}

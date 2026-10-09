#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <unordered_map>

#include <HodEngine/Core/Singleton.hpp>
#include <HodEngine/Core/UID.hpp>

#include "HodEngine/RHI/GraphicsPipeline.hpp"
#include "HodEngine/Core/StaticArray.hpp"

#include <utility>

namespace hod::inline rhi
{
	class MaterialInstance;
}

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_API MaterialManager
	{
		_Singleton(MaterialManager)

	public:
		enum class BuiltinMaterial : uint32_t
		{
			P2f_Unlit_Line,
			P2f_Unlit_Triangle,
			P2f_Unlit_TriangleFan,
			P2f_Unlit_Line_TriangleFan,
			P2f_Unlit_Line_LineStrip,
			P2fT2f_Texture_Unlit,
			P2fT2f_Texture_Unlit_Color,
			P2fC4f_Unlit_Fill_Triangle,
			P2fC4f_Unlit_Fill_TriangleFan,
			P2fC4f_Unlit_Line_TriangleFan,
			P2fC4f_Unlit_Line_Line,

			Count,
		};

	public:
		~MaterialManager();
		void Clear();

		const GraphicsPipeline* GetBuiltinMaterial(BuiltinMaterial buildMaterial);
		const MaterialInstance* GetBuiltinMaterialDefaultInstance(BuiltinMaterial buildMaterial);

		UID CreateMaterial(const String& shaderName, GraphicsPipeline::PolygonMode polygonMode = GraphicsPipeline::PolygonMode::Fill, GraphicsPipeline::Topololy topololy = GraphicsPipeline::Topololy::TRIANGLE,
		                   bool useDepth = true);

	private:
		StaticArray<GraphicsPipeline*, static_cast<uint32_t>(BuiltinMaterial::Count)> _builtinMaterials = {nullptr}; // c++23 std::to_underlying
		StaticArray<MaterialInstance*, static_cast<uint32_t>(BuiltinMaterial::Count)> _builtinDefaultInstances = {nullptr};
		StaticArray<Shader*, static_cast<uint32_t>(BuiltinMaterial::Count)>   _builtinVertexShaders = {nullptr};
		StaticArray<Shader*, static_cast<uint32_t>(BuiltinMaterial::Count)>   _builtinFragmentShaders = {nullptr};
	};
}

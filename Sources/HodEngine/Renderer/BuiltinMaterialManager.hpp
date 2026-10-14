#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <unordered_map>

#include <HodEngine/Core/Singleton.hpp>

#include "HodEngine/Renderer/Material.hpp"
#include "HodEngine/Core/StaticArray.hpp"

#include <utility>

namespace hod::inline renderer
{
	class MaterialInstance;
}

namespace hod::inline renderer
{
	/// @brief The materials of the engine itself, built from shaders embedded in it, each with a default instance.
	/// The materials of a project are MaterialResources.
	class HOD_RENDERER_API BuiltinMaterialManager
	{
		_Singleton(BuiltinMaterialManager)

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
		~BuiltinMaterialManager();
		void Clear();

		const Material* GetBuiltinMaterial(BuiltinMaterial buildMaterial);
		const MaterialInstance* GetBuiltinMaterialDefaultInstance(BuiltinMaterial buildMaterial);

	private:
		StaticArray<Material*, static_cast<uint32_t>(BuiltinMaterial::Count)> _builtinMaterials = {nullptr}; // c++23 std::to_underlying
		StaticArray<MaterialInstance*, static_cast<uint32_t>(BuiltinMaterial::Count)> _builtinDefaultInstances = {nullptr};
		StaticArray<Shader*, static_cast<uint32_t>(BuiltinMaterial::Count)>   _builtinVertexShaders = {nullptr};
		StaticArray<Shader*, static_cast<uint32_t>(BuiltinMaterial::Count)>   _builtinFragmentShaders = {nullptr};
	};
}

#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Core/Type.hpp>
#include <HodEngine/Core/Math/Vector2.hpp>
#include <HodEngine/Core/Math/Vector4.hpp>
#include <HodEngine/Core/Resource/WeakResource.hpp>
#include <memory>
#include <vector>
#include <unordered_map>

#include "HodEngine/Editor/Importer/MaterialImporter.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::renderer
{
	class MaterialResource;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API MaterialEditorTab : public EditorTab
	{
	public:

		struct ShaderParamScalar : renderer::ShaderParameter
		{
			union
			{
				bool boolValue;
				int intValue;
				float floatValue;
			} _value;
		};

		struct ShaderParamTexture : renderer::ShaderParameter
		{
			WeakResource<renderer::TextureResource> _value;
		};

		struct ShaderParamVec2 : renderer::ShaderParameter
		{
			Vector2 _value;
		};

		struct ShaderParamVec4 : renderer::ShaderParameter
		{
			Vector4 _value;
		};

	public:
				MaterialEditorTab(std::shared_ptr<Asset> asset);
				~MaterialEditorTab() override;

		std::shared_ptr<renderer::MaterialResource>	GetMaterial() const;
		MaterialAsset&								GetMaterialAsset();

		std::vector<ShaderParamScalar>&				GetScalarParameters();
		std::vector<ShaderParamTexture>&			GetTextureParameters();
		std::vector<ShaderParamVec2>&				GetVector2Parameters();
		std::vector<ShaderParamVec4>&				GetVector4Parameters();

		float										GetZoomFactor() const;
		void										SetZoomFactor(float zoomFactor);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;

	private:

		float _zoomFactor = 1.0f;

		std::shared_ptr<renderer::MaterialResource>	_material;

		std::vector<renderer::ShaderParameter>	_parameters;
		std::vector<ShaderParamScalar>			_scalarParameters;
		std::vector<ShaderParamTexture>			_textureParameters;
		std::vector<ShaderParamVec2>			_vec2Parameters;
		std::vector<ShaderParamVec4>			_vec4Parameters;

		MaterialAsset						_materialAsset;
	};
}

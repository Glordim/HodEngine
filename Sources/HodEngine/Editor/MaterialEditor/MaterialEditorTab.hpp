#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Core/TypeTrait.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/GameSystems/Resource/WeakResource.hpp>
#include <memory>
#include "HodEngine/Core/Vector.hpp"
#include <unordered_map>

#include "HodEngine/Editor/MaterialEditor/MaterialImporter.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

#include "HodEngine/Core/Document/Document.hpp"
#include "HodEngine/Renderer/RHI/Material.hpp"

namespace hod::inline renderer
{
	class MaterialResource;
}

namespace hod::inline editor
{
	class HOD_EDITOR_API MaterialSettings
	{
		REFLECTED_CLASS_NO_VIRTUAL(MaterialSettings)

		Material::PolygonMode		_polygonMode = Material::PolygonMode::Fill;
		Material::Topololy			_topololy = Material::Topololy::TRIANGLE;

		Document					_defaultInstanceParams;
	};

	/// @brief 
	class HOD_EDITOR_API MaterialEditorTab : public EditorTab
	{
	public:

		struct ShaderParamScalar : ShaderParameter
		{
			union
			{
				bool boolValue;
				int intValue;
				float floatValue;
			} _value;
		};

		struct ShaderParamTexture : ShaderParameter
		{
			WeakResource<TextureResource> _value;
		};

		struct ShaderParamVec2 : ShaderParameter
		{
			Vector2 _value;
		};

		struct ShaderParamVec4 : ShaderParameter
		{
			Vector4 _value;
		};

	public:
				MaterialEditorTab(std::shared_ptr<Asset> asset);
				~MaterialEditorTab() override;

		std::shared_ptr<MaterialResource>	GetMaterial() const;

		MaterialSettings&	GetMaterialSettings() { return _materialSettings; }

		Vector<ShaderParamScalar>&					GetScalarParameters();
		Vector<ShaderParamTexture>&					GetTextureParameters();
		Vector<ShaderParamVec2>&					GetVector2Parameters();
		Vector<ShaderParamVec4>&					GetVector4Parameters();

		float										GetZoomFactor() const;
		void										SetZoomFactor(float zoomFactor);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		float _zoomFactor = 1.0f;
		
		MaterialSettings _materialSettings;

		std::shared_ptr<MaterialResource>	_material;

		Vector<ShaderParameter>	_parameters;
		Vector<ShaderParamScalar>			_scalarParameters;
		Vector<ShaderParamTexture>			_textureParameters;
		Vector<ShaderParamVec2>				_vec2Parameters;
		Vector<ShaderParamVec4>				_vec4Parameters;
	};
}

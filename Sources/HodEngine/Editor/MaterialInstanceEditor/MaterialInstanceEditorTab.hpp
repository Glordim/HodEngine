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

#include "HodEngine/Editor/Importer/MaterialInstanceImporter.hpp"
#include "HodEngine/Renderer/Resource/MaterialSerializationHelper.hpp"
#include "HodEngine/Renderer/Resource/TextureResource.hpp"

namespace hod::inline renderer
{
	class MaterialInstanceResource;
}

namespace hod::inline editor
{
	/// @brief 
	class HOD_EDITOR_API MaterialInstanceEditorTab : public EditorTab
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
				MaterialInstanceEditorTab(std::shared_ptr<Asset> asset);
				~MaterialInstanceEditorTab() override;

		std::shared_ptr<MaterialInstanceResource>	GetMaterialInstance() const;
		MaterialInstanceAsset&								GetMaterialInstanceAsset();

		Vector<ShaderParamScalar>&				GetScalarParameters();
		Vector<ShaderParamTexture>&			GetTextureParameters();
		Vector<ShaderParamVec2>&				GetVector2Parameters();
		Vector<ShaderParamVec4>&				GetVector4Parameters();

		float										GetZoomFactor() const;
		void										SetZoomFactor(float zoomFactor);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;

	private:

		float _zoomFactor = 1.0f;

		std::shared_ptr<MaterialInstanceResource>	_materialInstance;

		Vector<ShaderParameter>	_parameters;
		Vector<ShaderParamScalar>			_scalarParameters;
		Vector<ShaderParamTexture>			_textureParameters;
		Vector<ShaderParamVec2>			_vec2Parameters;
		Vector<ShaderParamVec4>			_vec4Parameters;

		MaterialInstanceAsset					_materialInstanceAsset;
	};
}

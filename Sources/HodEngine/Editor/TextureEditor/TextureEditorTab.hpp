#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"

#include <HodEngine/Core/Type.hpp>
#include <memory>

namespace hod::renderer
{
	class TextureResource;
}

namespace hod::editor
{
	/// @brief 
	class HOD_EDITOR_API TextureEditorTab : public EditorTab
	{
	public:
		enum class ColorComponent : uint8_t
		{
			Red = 1 << 0,
			Green = 1 << 1,
			Blue = 1 << 2,
			Alpha = 1 << 3,
		};

	public:
				TextureEditorTab(std::shared_ptr<Asset> asset);
				~TextureEditorTab() override;

		std::shared_ptr<renderer::TextureResource>	GetTexture() const;
		float										GetZoomFactor() const;
		void										SetZoomFactor(float zoomFactor);

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;

	private:

		std::underlying_type_t<ColorComponent>	_colorComponentMask = std::to_underlying(ColorComponent::Red) | std::to_underlying(ColorComponent::Green) | std::to_underlying(ColorComponent::Blue) | std::to_underlying(ColorComponent::Alpha);

		uint8_t	_mipmapLevel = 0;
		uint8_t	_maxMipmapLevel = 0;
		float _zoomFactor = 1.0f;

		std::shared_ptr<renderer::TextureResource>	_texture;
	};
}

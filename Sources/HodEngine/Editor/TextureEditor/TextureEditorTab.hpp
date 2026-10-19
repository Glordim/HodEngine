#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureCooker.hpp"

#include <HodEngine/Core/TypeTrait.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <memory>

namespace hod::inline renderer
{
	class TextureResource;
}

namespace hod::inline editor
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

		std::shared_ptr<TextureResource>	GetTexture() const;
		float								GetZoomFactor() const;
		void								SetZoomFactor(float zoomFactor); // the one shown, the target is left as is
		float								GetTargetZoomFactor() const;
		void								SetTargetZoomFactor(float zoomFactor); // the one the zoom shown eases towards (mouse wheel)
		const Vector2&						GetPanOffset() const;
		void								SetPanOffset(const Vector2& panOffset);
		uint8_t								GetColorComponentMask() const { return _colorComponentMask; }

		TextureSettings&					GetTextureSettings() { return _textureSettings; }
		void								ApplySamplerStates();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		bool	OnSave() override;

	private:

		std::underlying_type_t<ColorComponent>	_colorComponentMask = std::to_underlying(ColorComponent::Red) | std::to_underlying(ColorComponent::Green) | std::to_underlying(ColorComponent::Blue) | std::to_underlying(ColorComponent::Alpha);

		uint8_t	_mipmapLevel = 0;
		uint8_t	_maxMipmapLevel = 0;
		static constexpr float MinZoomFactor = 0.01f;
		static constexpr float MaxZoomFactor = 64.0f; // texels big enough to tell them apart

		float _zoomFactor = 1.0f;
		float _targetZoomFactor = 1.0f;
		Vector2 _panOffset = Vector2(0.0f, 0.0f); // in pixels of the viewport, from the image centered in it (Y down)

		TextureSettings _textureSettings;
		TextureSettings _savedSettings; // what the resource is reverted to if the tab is closed with unsaved live edits

		std::shared_ptr<TextureResource>	_texture;
	};
}

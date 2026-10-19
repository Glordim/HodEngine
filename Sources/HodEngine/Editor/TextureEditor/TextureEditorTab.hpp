#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/EditorTab.hpp"
#include "HodEngine/Editor/TextureEditor/TextureCooker.hpp"

#include <HodEngine/Core/TypeTrait.hpp>
#include <HodEngine/Math/Color.hpp>
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

		/// @brief Color of the lines between the texels
		enum class GridStyle : uint8_t
		{
			TwoTone, // a dark and a light line side by side: one of the two stands out over any texel
			Dark,
			Light,
			Color,

			Count,
		};

		/// @brief How the grid between the texels is shown, for the time the tab is open
		struct GridSettings
		{
			bool      _visible = true;
			GridStyle _style = GridStyle::TwoTone;
			Color     _color = Color(0.0f, 0.8f, 1.0f, 1.0f); // GridStyle::Color
			float     _opacity = 0.35f;
			float     _minZoomFactor = 6.0f; // fades in from it, fully visible at twice it
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
		bool								IsRecentering() const { return _recentering; }
		void								SetRecentering(bool recentering) { _recentering = recentering; }
		uint8_t								GetColorComponentMask() const { return _colorComponentMask; }
		const GridSettings&					GetGridSettings() const;

		TextureSettings&					GetTextureSettings() { return _textureSettings; }
		void								ApplySamplerStates();

	protected:

		void	CreateDefaultLayout() override;
		bool	DrawContent() override;
		void	DrawMenuBar() override;
		void	DrawMenuBarPopups() override;
		bool	OnSave() override;

	private:

		void	DrawGridSettingsPopup();

	private:

		std::underlying_type_t<ColorComponent>	_colorComponentMask = std::to_underlying(ColorComponent::Red) | std::to_underlying(ColorComponent::Green) | std::to_underlying(ColorComponent::Blue) | std::to_underlying(ColorComponent::Alpha);

		uint8_t	_mipmapLevel = 0;
		uint8_t	_maxMipmapLevel = 0;
		static constexpr float MinZoomFactor = 0.01f;
		static constexpr float MaxZoomFactor = 64.0f; // texels big enough to tell them apart

		float _zoomFactor = 1.0f;
		float _targetZoomFactor = 1.0f;
		Vector2 _panOffset = Vector2(0.0f, 0.0f); // in pixels of the viewport, from the image centered in it (Y down)
		bool    _recentering = false;             // the view is reset: the pan eases towards 0 along with the zoom towards its target

		GridSettings _gridSettings;

		TextureSettings _textureSettings;
		TextureSettings _savedSettings; // what the resource is reverted to if the tab is closed with unsaved live edits

		std::shared_ptr<TextureResource>	_texture;
	};
}

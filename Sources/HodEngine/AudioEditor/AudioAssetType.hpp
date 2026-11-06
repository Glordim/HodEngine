#pragma once
#include "HodEngine/AudioEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief The sounds, imported from .wav files
	class HOD_AUDIO_EDITOR_API AudioAssetType : public AssetType
	{
		HOD_ASSET_TYPE("Audio", "audio")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		Texture*                    GetIcon() const override;
	};
}

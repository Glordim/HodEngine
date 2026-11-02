#include "HodEngine/AudioEditor/Pch.hpp"
#include "HodEngine/AudioEditor/AudioAssetType.hpp"
#include "HodEngine/AudioEditor/AudioCooker.hpp"
#include "HodEngine/AudioEditor/AudioEditorTab.hpp"

#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/Audio/AudioResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* AudioAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<AudioCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* AudioAssetType::GetCookedResource() const
	{
		return &AudioResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* AudioAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<AudioEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* AudioAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetAudioTexture();
	}
}

#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureAssetType.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureCooker.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureEditorTab.hpp"

#include <HodEngine/Renderer/Resource/TextureResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* TextureAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<TextureCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* TextureAssetType::GetCookedResource() const
	{
		return &TextureResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* TextureAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<TextureEditorTab>(asset);
	}
}

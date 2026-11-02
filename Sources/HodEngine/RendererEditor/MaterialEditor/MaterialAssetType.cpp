#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialAssetType.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialCooker.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialEditorTab.hpp"

#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/Renderer/Resource/MaterialResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* MaterialAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<MaterialCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* MaterialAssetType::GetCookedResource() const
	{
		return &MaterialResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* MaterialAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<MaterialEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* MaterialAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetMaterialTexture();
	}
}

#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceAssetType.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceCooker.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceEditorTab.hpp"

#include "HodEngine/Editor/Editor.hpp"

#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>

namespace hod::inline editor
{
	/// @brief
	/// @return
	Cooker* FontFaceAssetType::CreateCooker() const
	{
		return DefaultAllocator::GetInstance().New<FontFaceCooker>();
	}

	/// @brief
	/// @return
	const ReflectionDescriptor* FontFaceAssetType::GetCookedResource() const
	{
		return &FontFaceResource::GetReflectionDescriptor();
	}

	/// @brief
	/// @param asset
	/// @return
	EditorTab* FontFaceAssetType::CreateEditorTab(std::shared_ptr<Asset> asset) const
	{
		return DefaultAllocator::GetInstance().New<FontFaceEditorTab>(asset);
	}

	/// @brief
	/// @return
	Texture* FontFaceAssetType::GetIcon() const
	{
		return Editor::GetInstance()->GetFontTexture();
	}
}

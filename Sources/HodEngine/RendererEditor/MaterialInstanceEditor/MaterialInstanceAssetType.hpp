#pragma once
#include "HodEngine/RendererEditor/Export.hpp"

#include "HodEngine/Editor/AssetType.hpp"

namespace hod::inline editor
{
	/// @brief A material with its own parameter values
	class HOD_RENDERER_EDITOR_API MaterialInstanceAssetType : public AssetType
	{
		HOD_ASSET_TYPE("MaterialInstance")

	public:
		Cooker*                     CreateCooker() const override;
		const ReflectionDescriptor* GetCookedResource() const override;
		EditorTab*                  CreateEditorTab(std::shared_ptr<Asset> asset) const override;
		void                        OnRegistered(EditorModuleRegistration& registration) override;
	};
}

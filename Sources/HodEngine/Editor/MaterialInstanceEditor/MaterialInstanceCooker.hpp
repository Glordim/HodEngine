#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

#include "HodEngine/GameSystems/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialResource.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API MaterialInstanceAsset
	{
		REFLECTED_CLASS_NO_PARENT(MaterialInstanceAsset)

		virtual ~MaterialInstanceAsset() = default;

		WeakResource<MaterialResource>	_material;
		Document									_params;
	};

	class HOD_EDITOR_API MaterialInstanceCooker : public Cooker
	{
	public:

									MaterialInstanceCooker() = default;
									MaterialInstanceCooker(const MaterialInstanceCooker&) = delete;
									MaterialInstanceCooker(MaterialInstanceCooker&&) = delete;
									~MaterialInstanceCooker() override = default;

		MaterialInstanceCooker&		operator = (const MaterialInstanceCooker&) = delete;
		MaterialInstanceCooker&		operator = (MaterialInstanceCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

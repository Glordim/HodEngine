#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API UIPrefabCooker : public Cooker
	{
	public:

							UIPrefabCooker() = default;
							UIPrefabCooker(UIPrefabCooker&) = delete;
							UIPrefabCooker(UIPrefabCooker&&) = delete;
							~UIPrefabCooker() override = default;

		UIPrefabCooker&		operator = (UIPrefabCooker&) = delete;
		UIPrefabCooker&		operator = (UIPrefabCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

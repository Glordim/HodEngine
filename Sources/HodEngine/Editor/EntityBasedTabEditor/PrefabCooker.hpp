#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API PrefabCooker : public Cooker
	{
	public:

							PrefabCooker() = default;
							PrefabCooker(PrefabCooker&) = delete;
							PrefabCooker(PrefabCooker&&) = delete;
							~PrefabCooker() override = default;

		PrefabCooker&		operator = (PrefabCooker&) = delete;
		PrefabCooker&		operator = (PrefabCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

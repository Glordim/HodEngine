#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API DataTableCooker : public Cooker
	{
	public:

									DataTableCooker() = default;
									DataTableCooker(DataTableCooker&) = delete;
									DataTableCooker(DataTableCooker&&) = delete;
									~DataTableCooker() override = default;

		DataTableCooker&		operator = (DataTableCooker&) = delete;
		DataTableCooker&		operator = (DataTableCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

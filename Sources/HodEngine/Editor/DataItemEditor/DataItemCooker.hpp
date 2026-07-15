#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API DataItemCooker : public Cooker
	{
	public:

									DataItemCooker() = default;
									DataItemCooker(DataItemCooker&) = delete;
									DataItemCooker(DataItemCooker&&) = delete;
									~DataItemCooker() override = default;

		DataItemCooker&		operator = (DataItemCooker&) = delete;
		DataItemCooker&		operator = (DataItemCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

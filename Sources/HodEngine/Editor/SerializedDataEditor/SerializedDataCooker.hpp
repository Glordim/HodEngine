#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API SerializedDataCooker : public Cooker
	{
	public:

									SerializedDataCooker() = default;
									SerializedDataCooker(SerializedDataCooker&) = delete;
									SerializedDataCooker(SerializedDataCooker&&) = delete;
									~SerializedDataCooker() override = default;

		SerializedDataCooker&		operator = (SerializedDataCooker&) = delete;
		SerializedDataCooker&		operator = (SerializedDataCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

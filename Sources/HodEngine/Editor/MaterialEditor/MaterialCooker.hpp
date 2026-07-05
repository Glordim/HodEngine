#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API MaterialCooker : public Cooker
	{
	public:

							MaterialCooker() = default;
							MaterialCooker(const MaterialCooker&) = delete;
							MaterialCooker(MaterialCooker&&) = delete;
							~MaterialCooker() override = default;

		MaterialCooker&	operator = (const MaterialCooker&) = delete;
		MaterialCooker&	operator = (MaterialCooker&&) = delete;

	protected:

		bool	FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

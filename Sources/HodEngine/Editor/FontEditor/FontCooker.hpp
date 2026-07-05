#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API FontCooker : public Cooker
	{
	public:

							FontCooker() = default;
							FontCooker(const FontCooker&) = delete;
							FontCooker(FontCooker&&) = delete;
							~FontCooker() override = default;

		FontCooker&		operator = (const FontCooker&) = delete;
		FontCooker&		operator = (FontCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

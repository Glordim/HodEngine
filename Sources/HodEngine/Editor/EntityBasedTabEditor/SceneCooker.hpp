#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API SceneCooker : public Cooker
	{
	public:

							SceneCooker() = default;
							SceneCooker(SceneCooker&) = delete;
							SceneCooker(SceneCooker&&) = delete;
							~SceneCooker() override = default;

		SceneCooker&		operator = (SceneCooker&) = delete;
		SceneCooker&		operator = (SceneCooker&&) = delete;

	protected:

		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

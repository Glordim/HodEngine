#pragma once
#include "HodEngine/Editor/Export.hpp"

#include "HodEngine/Editor/Cooker/Cooker.hpp"

namespace hod::inline editor
{
	class HOD_EDITOR_API AudioCooker : public Cooker
	{
	public:
		AudioCooker();
		AudioCooker(const AudioCooker&) = delete;
		AudioCooker(AudioCooker&&) = delete;
		~AudioCooker() override = default;

		AudioCooker& operator=(const AudioCooker&) = delete;
		AudioCooker& operator=(AudioCooker&&) = delete;

	protected:
		bool FillDataBlock(const Asset& asset, uint32_t platforms, uint8_t configs, uint32_t languages) override;
	};
}

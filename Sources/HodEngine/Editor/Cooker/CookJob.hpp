#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/GameSystems/Job/Job.hpp>
#include <cstdint>

namespace hod::inline editor
{
	class Asset;

	class HOD_EDITOR_API CookJob : public Job
	{
	public:
		CookJob(const Path& asset, uint8_t platforms, uint8_t configs, uint32_t languages);
		CookJob(const Asset& asset, uint8_t platforms, uint8_t configs, uint32_t languages);

	protected:
		void Execution() override;

	private:
		Path _assetPath;
		const Asset* _asset = nullptr;
		uint64_t _taskId = 0;

		uint8_t _platforms = 0;
		uint8_t _configs = 0;
		uint32_t _languages = 0;
	};
}

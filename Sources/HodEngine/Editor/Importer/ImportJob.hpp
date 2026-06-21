#pragma once
#include "HodEngine/Editor/Export.hpp"

#include <HodEngine/Core/FileSystem/Path.hpp>
#include <HodEngine/GameSystems/Job/Job.hpp>
#include <cstdint>

namespace hod::inline editor
{
	class HOD_EDITOR_API ImportJob : public Job
	{
	public:
		ImportJob(const Path& sourceFilePath, const Path& destinationDirPath);

	protected:
		void Execution() override;

	private:
		Path _sourceFilePath;
		Path _destinationDirPath;
		uint64_t _taskId = 0;
	};
}

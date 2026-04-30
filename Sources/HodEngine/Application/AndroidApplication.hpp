#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplication.hpp"

struct android_app;

namespace hod::inline application
{
	/// @brief
	class HOD_APPLICATION_API AndroidApplication : public GraphicApplication
	{
	public:
		bool Run(android_app* androidApp);

	protected:

		void ConfigureFileSystem(FileSystemConfig& fileSystemConfig) override;

	private:
		android_app* _androidApp = nullptr;
	};
}

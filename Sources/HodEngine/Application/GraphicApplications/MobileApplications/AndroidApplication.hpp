#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/GraphicApplications/GraphicApplication.hpp"

struct android_app;

namespace hod::application
{
	/// @brief 
	class HOD_APPLICATION_API AndroidApplication : public GraphicApplication
	{
	public:

		bool			Init(android_app* androidApp);
		bool			Run() override;

	public:

		static void		CommandHandleStatic(android_app* androidApp, int32_t command);
		void			CommandHandle(int32_t command);

	protected:

		bool			Init(const ArgumentParser& argumentParser) override { return false; }

	private:

		android_app*	_androidApp = nullptr;
	};
}

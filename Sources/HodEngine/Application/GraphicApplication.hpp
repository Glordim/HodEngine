#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/Application.hpp"

#include <stdint.h>

namespace hod::inline window
{
	class Window;
}

namespace hod::inline application
{
	/// @brief
	class HOD_APPLICATION_API GraphicApplication : public Application
	{
	protected:
		bool RunInternal() override;

		bool InitAudio();
		bool TerminateAudio();

		bool InitWindow();
		bool TerminateWindow();

		bool InitInput();
		bool TerminateInput();

		bool InitRenderer();
		bool TerminateRenderer();

		bool InitImGui();
		bool TerminateImGui();

	private:
		static int EngineLoopEntry(void* data);
		int        EngineLoop();

	protected:
		Window* _window = nullptr;
	};
}

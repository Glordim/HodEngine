#pragma once
#include "HodEngine/Application/Export.hpp"

#include "HodEngine/Application/Application.hpp"

#include <stdint.h>

namespace hod::window
{
	class Window;
}

namespace hod::application
{
	/// @brief
	class HOD_APPLICATION_API GraphicApplication : public Application
	{
		_SingletonOverride(GraphicApplication)

	public:
		bool Init(const ArgumentParser& argumentParser) override;
		void Terminate() override;
		bool Run() override;

		window::Window* GetWindow() const;

	private:
		static int EngineLoopEntry(void* data);
		int        EngineLoop();

	protected:
		window::Window* _window = nullptr;
	};
}

#include "GraphicApplication.inl"

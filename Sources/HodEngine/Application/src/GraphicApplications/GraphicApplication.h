#pragma once

#include "../Application.h"

#include <stdint.h>

#include <HodEngine/Renderer/src/RHI/Context.h>

namespace hod::window
{
	class Window;
}

namespace hod::application
{
	/// @brief 
	class GraphicApplication : public Application
	{
		_Singleton(GraphicApplication)

						GraphicApplication() = default;

	public:

		bool			Init(int argc, char** argv) override;
		bool			Run() override;

		window::Window*	GetWindow() const;

	private:

		window::Window*	_window = nullptr;
	};
}

#include "GraphicApplication.inl"

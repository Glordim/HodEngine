#pragma once

#include "HodEngine/Application/Application.h"

#include <stdint.h>

#include <HodEngine/Renderer/RHI/Context.h>

namespace hod::window
{
	class Window;
}

namespace hod::application
{
	/// @brief 
	class GraphicApplication : public Application
	{
		_SingletonOverride(GraphicApplication)

	public:

		bool			Init(const core::ArgumentParser& argumentParser) override;
		bool			Run() override;

		window::Window*	GetWindow() const;

	private:

		window::Window*	_window = nullptr;
	};
}

#include "GraphicApplication.inl"

#pragma once

#include "HodEngine/Application/Application.hpp"

#include <stdint.h>

#include <HodEngine/Renderer/RHI/Context.hpp>

namespace hod::window
{
	class Window;
}

namespace hod::application
{
	/// @brief 
	class HOD_API GraphicApplication : public Application
	{
		_SingletonOverride(GraphicApplication)

	public:

		bool			Init(const ArgumentParser& argumentParser) override;
		bool			Run() override;

		window::Window*	GetWindow() const;

	private:

		window::Window*	_window = nullptr;
	};
}

#include "GraphicApplication.inl"

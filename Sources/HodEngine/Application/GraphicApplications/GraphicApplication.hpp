#pragma once
#include "HodEngine/Application/Export.hpp"

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
	class HOD_APPLICATION_API GraphicApplication : public Application
	{
		_SingletonOverride(GraphicApplication)

	public:

		bool			Init(const ArgumentParser& argumentParser) override;
		void			Terminate() override;
		bool			Run() override;

		window::Window*	GetWindow() const;

	protected:

		window::Window*	_window = nullptr;
	};
}

#include "GraphicApplication.inl"

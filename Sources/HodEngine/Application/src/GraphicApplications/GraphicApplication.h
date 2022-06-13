#pragma once

#include "../Application.h"

#include <stdint.h>

#include <HodEngine/Renderer/src/RHI/Context.h>

namespace HOD
{
	class Window;

	/// @brief 
	class GraphicApplication : public Application
	{
		_Singleton(GraphicApplication)

						GraphicApplication() = default;

	public:

		bool			Init(int argc, char** argv) override;
		bool			Run() override;

		Window*			GetWindow() const;

	private:

		Window*			_window = nullptr;
	};
}

#include "GraphicApplication.inl"

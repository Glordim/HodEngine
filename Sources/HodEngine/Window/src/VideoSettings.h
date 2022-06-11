#pragma once

namespace HOD
{
	namespace APPLICATION
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		enum FullscreenMode
		{
			Windowed,
			FullscreenWindowed,
			Fullscreen
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct Resolution
		{
			size_t width = 800;
			size_t height = 600;
			size_t refreshRate = 60;
			size_t bitPerPixel = 8;
		};

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		struct VideoSettings
		{
			size_t monitor = 0;
			Resolution resolution;
			FullscreenMode fullscreenMode = FullscreenMode::Windowed;
		};
	}
}

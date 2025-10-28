#include "HodEngine/Window/Pch.hpp"
#include "HodEngine/Window/ScreenHelper.hpp"
#include "HodEngine/Window/VideoSettings.hpp"

#include "HodEngine/Core/Output/OutputService.hpp"

namespace hod::window
{
	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	size_t ScreenHelper::GetMonitorCount()
	{
		return 0; // SDL_GetNumVideoDisplays();
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool ScreenHelper::GetNativeResoltion(Resolution* nativeResolution, int monitor)
	{
		(void)nativeResolution;
		(void)monitor;
		/*
		if (nativeResolution == nullptr)
		{
		    return false;
		}

		SDL_DisplayMode desktopMode;

		if (SDL_GetDesktopDisplayMode(monitor, &desktopMode) != 0)
		{
		    OUTPUT_ERROR("SDL: SDL_GetDesktopDisplayMode failed: {}", SDL_GetError());
		    return false;
		}

		nativeResolution->width = desktopMode.w;
		nativeResolution->height = desktopMode.h;
		nativeResolution->refreshRate = desktopMode.refresh_rate;
		nativeResolution->bitPerPixel = SDL_BITSPERPIXEL(desktopMode.format);

		return true;
		*/
		return false;
	}

	//-----------------------------------------------------------------------------
	//! @brief
	//-----------------------------------------------------------------------------
	bool ScreenHelper::GetSupportedResoltions(Vector<Resolution>* supportedResolution, int monitor)
	{
		(void)supportedResolution;
		(void)monitor;
		/*
		if (supportedResolution == nullptr)
		{
		    return false;
		}

		int modeCount = SDL_GetNumDisplayModes(monitor);

		if (modeCount == 0)
		{
		    return false;
		}

		supportedResolution->Resize(modeCount);

		for (int i = 0; i < modeCount; ++i)
		{
		    SDL_DisplayMode mode;

		    if (SDL_GetDisplayMode(monitor, i, &mode) != 0)
		    {
		        OUTPUT_ERROR("SDL: SDL_GetDisplayMode failed: {}", SDL_GetError());
		        return 1;
		    }

		    Resolution& resolution = supportedResolution->at(i);

		    resolution.width = mode.w;
		    resolution.height = mode.h;
		    resolution.refreshRate = mode.refresh_rate;
		    resolution.bitPerPixel = SDL_BITSPERPIXEL(mode.format);
		}

		return true;
		*/

		return false;
	}
}

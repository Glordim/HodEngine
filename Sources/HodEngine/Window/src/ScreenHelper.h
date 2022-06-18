#pragma once

#include <vector>
#include <cstddef>

namespace hod::window
{
	struct Resolution;

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class ScreenHelper
	{
	public:

		static size_t	GetMonitorCount();
		static bool		GetNativeResoltion(Resolution* nativeResolution, int monitor = 0);
		static bool		GetSupportedResoltions(std::vector<Resolution>* supportedResolution, int monitor = 0);

	private:

		ScreenHelper() = delete;
		ScreenHelper(const ScreenHelper&) = delete;
		ScreenHelper(ScreenHelper&&) = delete;
		~ScreenHelper() = delete;

		void			operator=(const ScreenHelper&) = delete;
		void			operator=(ScreenHelper&&) = delete;
	};
}

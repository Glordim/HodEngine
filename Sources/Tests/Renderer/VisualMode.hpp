#pragma once

#include <cstdint>

namespace hod::inline window
{
	class Window;
}

// ============================================================================
// Opt-in mode for a human to look at what the tests render:
//   TestsRenderer --visual [--visual-delay=<milliseconds>]
// A window shows the last frame of each test for the given delay.
// The tests themselves still render offscreen, exactly as without the flag.
// ============================================================================

struct VisualMode
{
	bool         _enabled = false;
	uint32_t     _delayMs = 1000;
	hod::Window* _window = nullptr;
};

VisualMode& GetVisualMode();

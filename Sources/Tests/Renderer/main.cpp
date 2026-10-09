#include <gtest/gtest.h>

#include "VisualMode.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/PlatformDisplayManager.hpp>

#include <cstdlib>
#include <cstring>

VisualMode& GetVisualMode()
{
	static VisualMode visualMode;
	return visualMode;
}

// ============================================================================
// Owns the Renderer (and its RhiDevice) for the whole run.
// No presentation surface is created: everything is rendered to RenderTargets.
// In visual mode a window gets a surface, but it is not made the Renderer's main one,
// so that frames which don't target it are not affected by it.
// ============================================================================

class RendererEnvironment : public ::testing::Environment
{
public:
	void SetUp() override
	{
		VisualMode& visualMode = GetVisualMode();
		if (visualMode._enabled)
		{
			ASSERT_TRUE(PlatformDisplayManager::CreateInstance()->Initialize()) << "Unable to init the DisplayManager";
			visualMode._window = hod::DisplayManager::GetInstance()->GetMainWindow();

			hod::DesktopWindow* desktopWindow = static_cast<hod::DesktopWindow*>(visualMode._window);
			desktopWindow->SetSize(512, 512);
			desktopWindow->CenterToScreen();
		}

		hod::Renderer::CreateInstance();
		ASSERT_TRUE(hod::Renderer::GetInstance()->Init()) << "Unable to init the Renderer";

		if (visualMode._enabled)
		{
			ASSERT_NE(hod::RhiDevice::GetInstance()->CreatePresentationSurface(visualMode._window), nullptr);
		}
	}

	void TearDown() override
	{
		hod::RhiDevice::GetInstance()->WaitIdle();
		hod::Renderer::GetInstance()->Clear(); // releases the presentation surface too
		hod::Renderer::DestroyInstance();

		VisualMode& visualMode = GetVisualMode();
		if (visualMode._window != nullptr)
		{
			hod::DefaultAllocator::GetInstance().Delete(visualMode._window);
			visualMode._window = nullptr;
			PlatformDisplayManager::DestroyInstance();
		}
	}
};

int main(int argc, char** argv)
{
	::testing::InitGoogleTest(&argc, argv); // strips the flags it knows

	VisualMode& visualMode = GetVisualMode();
	for (int index = 1; index < argc; ++index)
	{
		const char* delayFlag = "--visual-delay=";
		if (std::strcmp(argv[index], "--visual") == 0)
		{
			visualMode._enabled = true;
		}
		else if (std::strncmp(argv[index], delayFlag, std::strlen(delayFlag)) == 0)
		{
			visualMode._enabled = true;
			visualMode._delayMs = (uint32_t)std::strtoul(argv[index] + std::strlen(delayFlag), nullptr, 10);
		}
	}

	::testing::AddGlobalTestEnvironment(new RendererEnvironment()); // owned by gtest
	return RUN_ALL_TESTS();
}

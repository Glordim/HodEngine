#include "Application.h"

#include "VideoSettings.h"

//#include "ImGui/imgui.h"
//#include "ImGui/imgui_impl_sdl.h"

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_vulkan.h>

namespace HOD
{
    namespace APPLICATION
    {
        Application::Application()
        {
            this->window = nullptr;
        }

        Application::~Application()
        {
            //ImGui_ImplSDL2_Shutdown();
            //ImGui::DestroyContext();

            if (this->window != nullptr)
            {
                SDL_DestroyWindow(this->window);
            }

            if (SDL_WasInit(0) != 0)
            {
                SDL_Quit();
            }
        }

        bool Application::Init()
        {
            if (SDL_Init(SDL_INIT_VIDEO) != 0)
            {
                fprintf(stderr, "SDL: Unable to initialize SDL: %s", SDL_GetError());
                return false;
            }

            /*
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();
            //ImGui::StyleColorsClassic();
            */
            return true;
        }

        bool Application::CreateWindowAndContext(const std::string& name, const VideoSettings& videoSettings)
        {
            Uint32 flags = 0;

            if (videoSettings.fullscreenMode == FullscreenMode::FullscreenWindowed)
                flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
            else if (videoSettings.fullscreenMode == FullscreenMode::Fullscreen)
                flags |= SDL_WINDOW_FULLSCREEN;

            this->window = SDL_CreateWindow("Toto",
                SDL_WINDOWPOS_CENTERED_DISPLAY((int)videoSettings.monitor),
                SDL_WINDOWPOS_CENTERED_DISPLAY((int)videoSettings.monitor),
                (int)videoSettings.resolution.width,
                (int)videoSettings.resolution.height,
                flags);

            if (this->window == nullptr)
            {
                fprintf(stderr, "SDL: Unable to create Window: %s", SDL_GetError());
                return false;
            }

            //ImGui_ImplSDL2_InitForVulkan(window);

            return true;
        }

        bool Application::Run()
        {
            PreRun();

            float dt = 0.0f;
            Uint32 lastTicks = SDL_GetTicks();

            bool shouldExit = false;

            while (shouldExit == false)
            {
                Uint32 ticks = SDL_GetTicks();
                float time = ticks / 1000.0f;

                dt = time - ((float)lastTicks / 1000.0f);
                if (dt > 0.2f)
                    dt = 0.2f;

                lastTicks = ticks;

                Loop(dt);
            }

            PostRun();

            return true;
        }

        void* Application::GetHwnd()
        {
            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            if (SDL_GetWindowWMInfo(window, &wmInfo) == SDL_FALSE)
            {
                fprintf(stderr, "SDL: Unable to get native Window instance!\n");
                return nullptr;
            }

            return wmInfo.info.win.window;
        }

        bool Application::GetExtensionRequiredToCreateVulkanSurface(std::vector<const char*>* extensionsRequiredBySDL)
        {
            unsigned int extensionsRequiredBySDLCount = 0;
            if (SDL_Vulkan_GetInstanceExtensions(window, &extensionsRequiredBySDLCount, nullptr) == SDL_FALSE)
            {
                fprintf(stderr, "SDL: Unable to get Extensions required to create VulkanSurface: %s\n", SDL_GetError());
                return false;
            }

            extensionsRequiredBySDL->resize(extensionsRequiredBySDLCount);
            if (SDL_Vulkan_GetInstanceExtensions(window, &extensionsRequiredBySDLCount, extensionsRequiredBySDL->data()) == SDL_FALSE)
            {
                fprintf(stderr, "SDL: Unable to get Extensions required to create VulkanSurface: %s\n", SDL_GetError());
                return false;
            }

            return true;
        }

        bool Application::CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface)
        {
            if (SDL_Vulkan_CreateSurface(window, instance, surface) == SDL_FALSE)
            {
                fprintf(stderr, "SDL: Unable to create Vulkan surface for SDL Window: %s\n", SDL_GetError());
                return false;
            }

            return true;
        }
    }
}

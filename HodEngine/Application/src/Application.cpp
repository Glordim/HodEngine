#include "Application.h"

#include "VideoSettings.h"

#include <Core/Src/Output.h>
#include <Core/Src/UID.h>

#include <ImGui/src/imgui.h>
#include <ImGui/src/imgui_impl_sdl.h>
#include <ImGui/src/ImGuizmo.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_vulkan.h>

namespace HOD
{
	template<>
	APPLICATION::Application* CORE::Singleton<APPLICATION::Application>::_instance = nullptr;

	namespace APPLICATION
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Application::Application()
		{
			_instance = this;

			_window = nullptr;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		Application::~Application()
		{
			//ImGui_ImplSDL2_Shutdown();
			//ImGui::DestroyContext();

			if (_window != nullptr)
			{
				SDL_DestroyWindow(_window);
			}

			if (SDL_WasInit(0) != 0)
			{
				SDL_Quit();
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Application::Init(int argc, char** argv)
		{
			if (argc > 1)
			{
				//MessageBox(NULL, "Attach debugger ?", "AttachMe", MB_OK);
			}

			if (SDL_Init(SDL_INIT_VIDEO) != 0)
			{
				OUTPUT_ERROR("SDL: Unable to initialize SDL: %s", SDL_GetError());
				return false;
			}

			// Setup Dear ImGui context
			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
			//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

			// Setup Dear ImGui style
			ImGui::StyleColorsDark();
			//ImGui::StyleColorsClassic();

			if (argc >= 3)
			{
				if (strcmp(argv[1], "-Editor") == 0)
				{
					_parentHwnd = (void*)atoll(argv[2]);
				}
				if (strcmp(argv[3], "-Port") == 0)
				{
					_editorPort = atoi(argv[4]);
				}
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Application::IsRunningInEditor() const
		{
			return _parentHwnd != 0;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		uint16_t Application::GetEditorPort() const
		{
			return _editorPort;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void* Application::GetHwnd() const
		{
			return _hwnd;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Application::CreateWindowAndContext(const std::string& name, const VideoSettings& videoSettings)
		{
			Uint32 flags = 0;

			Uint32 posX;
			Uint32 posY;
			Uint32 width;
			Uint32 height;

			if (IsRunningInEditor() == true)
			{
				posX = 0;
				posY = 0;
				width = 2;
				height = 2;
			}
			else
			{
				posX = SDL_WINDOWPOS_CENTERED_DISPLAY((int)videoSettings.monitor);
				posY = SDL_WINDOWPOS_CENTERED_DISPLAY((int)videoSettings.monitor);
				width = videoSettings.resolution.width;
				height = videoSettings.resolution.height;

				if (videoSettings.fullscreenMode == FullscreenMode::FullscreenWindowed)
				{
					flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				}
				else if (videoSettings.fullscreenMode == FullscreenMode::Fullscreen)
				{
					flags |= SDL_WINDOW_FULLSCREEN;
				}
			}

			flags |= SDL_WINDOW_VULKAN;

			_window = SDL_CreateWindow("Toto",
				posX,
				posY,
				width,
				height,
				flags);

			if (_window == nullptr)
			{
				OUTPUT_ERROR("SDL: Unable to create Window: %s", SDL_GetError());
				return false;
			}

			SDL_SysWMinfo wmInfo;
			SDL_VERSION(&wmInfo.version);
			if (SDL_GetWindowWMInfo(_window, &wmInfo) == SDL_FALSE)
			{
				OUTPUT_ERROR("SDL: Unable to get native Window instance!\n");
				return false;
			}

		#if defined(_WIN32)
			_hwnd = wmInfo.info.win.window;
		#elif defined(__linux__)
			// TODO
		#endif

			ImGui_ImplSDL2_InitForVulkan(_window);

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Application::Run(const CORE::UID& startingSceneUid)
		{
			if (PreRun() == false)
			{
				return 1;
			}

			LoadStartingScene(startingSceneUid);

			float dt = 0.0f;
			Uint32 lastTicks = SDL_GetTicks();

			bool shouldExit = false;

			while (shouldExit == false)
			{
				Uint32 ticks = SDL_GetTicks();
				float time = ticks / 1000.0f;

				dt = time - ((float)lastTicks / 1000.0f);
				if (dt > 0.2f)
				{
					dt = 0.2f;
				}

				lastTicks = ticks;

				SDL_Event event;
				while (SDL_PollEvent(&event))
				{
					ImGui_ImplSDL2_ProcessEvent(&event);
					if (event.type == SDL_QUIT)
					{
						shouldExit = true;
					}
				}

				ImGui_ImplSDL2_NewFrame(_window);
				ImGui::NewFrame();
				ImGuizmo::BeginFrame();

				if (Loop(dt) == false)
				{
					shouldExit = true;
				}
			}

			if (PostRun() == false)
			{
				return 1;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		int Application::GetWidth()
		{
			int w;
			int h;

			SDL_GetWindowSize(_window, &w, &h);

			return w;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		int Application::GetHeight()
		{
			int w;
			int h;

			SDL_GetWindowSize(_window, &w, &h);

			return h;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Application::GetExtensionRequiredToCreateVulkanSurface(std::vector<const char*>* extensionsRequiredBySDL)
		{
			unsigned int extensionsRequiredBySDLCount = 0;
			if (SDL_Vulkan_GetInstanceExtensions(_window, &extensionsRequiredBySDLCount, nullptr) == SDL_FALSE)
			{
				OUTPUT_ERROR("SDL: Unable to get Extensions required to create VulkanSurface: %s\n", SDL_GetError());
				return false;
			}

			extensionsRequiredBySDL->resize(extensionsRequiredBySDLCount);
			if (SDL_Vulkan_GetInstanceExtensions(_window, &extensionsRequiredBySDLCount, extensionsRequiredBySDL->data()) == SDL_FALSE)
			{
				OUTPUT_ERROR("SDL: Unable to get Extensions required to create VulkanSurface: %s\n", SDL_GetError());
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool Application::CreateVulkanSurface(VkInstance instance, VkSurfaceKHR* surface)
		{
			if (SDL_Vulkan_CreateSurface(_window, instance, surface) == SDL_FALSE)
			{
				OUTPUT_ERROR("SDL: Unable to create Vulkan surface for SDL Window: %s\n", SDL_GetError());
				return false;
			}

			return true;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void Application::SetCursorPosition(int x, int y)
		{
			SDL_WarpMouseInWindow(_window, x, y);
		}
	}
}

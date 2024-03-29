#if defined(PLATFORM_LINUX)

#include "HodEngine/Application/GraphicApplications/DesktopApplications/LinuxApplication.hpp"

namespace hod::application
{
	_SingletonOverrideConstructor(LinuxApplication)
	{
		
	}
}

#endif

/*
#include "Application.hpp"

#include "VideoSettings.hpp"

#include <Core/Output.hpp>
#include <Core/UID.hpp>

#include <Core/GenericManager.hpp>
#include <Core/Output.hpp>
#include <Core/ArgumentParser.hpp>
#include <Core/StringConversion.hpp>

#include <World/ActorReflection.hpp>
#include <World/ComponentFactory.hpp>
#include <World/Builtin.hpp>

#include <ImGui/imgui.hpp>
#include <ImGui/imgui_impl_sdl.hpp>
#include <ImGui/imgui_impl_vulkan.hpp>
#include <ImGui/ImGuizmo.hpp>

#include <SDL.hpp>
#include <SDL_syswm.hpp>
#include <SDL_vulkan.hpp>

namespace hod
{
	template<>
	APPLICATION::Application* Singleton<APPLICATION::Application>::_instance = nullptr;

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
		Application::InitResult Application::Init(int argc, char** argv)
		{
			ArgumentParser argParser;
			argParser.Register('\0', "toolDump", "Request dump reflection data (used by the editor) : [dir where store dumped files]", true);
			argParser.Register('\0', "toolParent", "Embed the Application as child (used by the editor) : [parent window handle]", true);
			argParser.Register('\0', "toolPort", "Run a Tcp server to comunicate with tools (used by the editor) : [port number]", true);
			if (argParser.Parse(argc, argv) == true)
			{
				const char* dumpPath = argParser.GetValue("toolDump");
				if (dumpPath != nullptr)
				{
					game::RegisterBuiltin();

					rapidjson::Document document;
					rapidjson::Value& rootNode = document.SetObject();

					rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

					document.AddMember("Version", rapidjson::StringRef("1.0"), allocator);

					if (game::ComponentReflection::GetInstance()->Dump(rootNode, allocator) == false)
					{
						return InitResult::Failure;
					}

					rapidjson::StringBuffer buf;
					rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buf);
					document.Accept(writer);

					std::ofstream stream;
					stream.open(dumpPath, std::ofstream::out);
					if (stream.is_open() == true)
					{
						const char* json = buf.GetString();
						size_t jsonSize = buf.GetSize();
						stream.write(json, jsonSize);
						stream.flush();
						stream.close();
					}
					else
					{
						// todo error
					}

					return InitResult::Quit;
				}

				const char* toolParentString = argParser.GetValue("toolParent");
				if (toolParentString != nullptr)
				{
					int64_t toolParentId;
					if (StringConversion::StringToInt64(toolParentString, toolParentId) == false)
					{
						return InitResult::Failure;
					}
					_editorHwnd = reinterpret_cast<void*>(toolParentId);
				}

				const char* toolPortString = argParser.GetValue("toolPort");
				if (toolPortString != nullptr)
				{
					if (StringConversion::StringToUInt16(toolPortString, _editorPort) == false)
					{
						return InitResult::Failure;
					}
				}
			}
			else
			{
				return InitResult::Failure;
			}

			if (SDL_Init(SDL_INIT_VIDEO) != 0)
			{
				OUTPUT_ERROR("SDL: Unable to initialize SDL: %s", SDL_GetError());
				return InitResult::Failure;
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

			return InitResult::Success;
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
		bool Application::Run(const UID& startingSceneUid)
		{
			if (PreRun() == false)
			{
				return 1;
			}

			//LoadStartingScene(startingSceneUid);

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
				ImGui_ImplVulkan_NewFrame();
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
*/
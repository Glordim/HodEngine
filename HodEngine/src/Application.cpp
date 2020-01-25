#include "Application.h"

#include <SDL.h>

#include "Renderer/GraphicsSettings.h"

#include "Renderer/Renderer.h"
#include "Renderer/Vulkan/RendererVulkan.h"
#include "Renderer/D3D12/RendererDirectX12.h"

#include "Game/Scene.h"
#include "Game/Component/CameraComponent.h"

#include <Physic/src/Physic.h>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl.h"
#include "ImGui/imgui_impl_vulkan.h"

#include "DebugLayer/DebugLayer.h"

namespace HOD
{
    Application::Application()
    {
        this->window = nullptr;
        this->renderer = nullptr;
    }

    Application::~Application()
    {
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();

        if (this->window != nullptr)
        {
            SDL_DestroyWindow(this->window);
        }

        if (SDL_WasInit(0) != 0)
        {
            SDL_Quit();
        }

        PHYSIC::Physic::DestroyInstance();
    }

    bool Application::Init()
    {
        if (SDL_Init(SDL_INIT_VIDEO) != 0)
        {
            fprintf(stderr, "SDL: Unable to initialize SDL: %s", SDL_GetError());
            return false;
        }

        PHYSIC::Physic* physic = PHYSIC::Physic::CreateInstance();
        if (physic->Init() == false)
        {
            return false;
        }

        DEBUG_LAYER::DebugLayer::CreateInstance();

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsClassic();

        return true;
    }

    bool Application::CreateWindowAndContext(const std::string& name, const GraphicsSettings& graphicsSettings)
    {
        Uint32 flags = 0;

        if (graphicsSettings.fullscreenType == GraphicsSettings::FullscreenType::FullscreenWindow)
            flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
        else if (graphicsSettings.fullscreenType == GraphicsSettings::FullscreenType::Fullscreen)
            flags |= SDL_WINDOW_FULLSCREEN;

        if (graphicsSettings.api == GraphicsSettings::API::Vulkan)
            flags |= SDL_WINDOW_VULKAN;

        //flags |= SDL_WINDOW_RESIZABLE;

        this->window = SDL_CreateWindow("Toto",
            SDL_WINDOWPOS_CENTERED_DISPLAY((int)graphicsSettings.monitor),
            SDL_WINDOWPOS_CENTERED_DISPLAY((int)graphicsSettings.monitor),
            (int)graphicsSettings.width,
            (int)graphicsSettings.height,
            flags);

        if (this->window == nullptr)
        {
            fprintf(stderr, "SDL: Unable to create Window: %s", SDL_GetError());
            return false;
        }

        if (graphicsSettings.api == GraphicsSettings::API::Vulkan)
        {
            ImGui_ImplSDL2_InitForVulkan(window);

            this->renderer = new RendererVulkan();
        }
        else if (graphicsSettings.api == GraphicsSettings::API::D3d12)
        {
            this->renderer = new RendererDirectX12();
        }
        else
        {
            return false;
        }

#ifdef NDEBUG
        bool enableValidationLayers = false;
#else
        bool enableValidationLayers = true;
#endif

        if (this->renderer->Init(this->window, enableValidationLayers) == false)
            return false;

        return true;
    }

    bool Application::Run(GAME::Scene* scene)
    {
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

            // Physic
            scene->simulatePhysic(dt);

            SDL_Event event;
            while (SDL_PollEvent(&event) != 0)
            {
                ImGui_ImplSDL2_ProcessEvent(&event);

                if (event.type == SDL_QUIT)
                    shouldExit = true;
                else if (event.type == SDL_WINDOWEVENT && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_HIDDEN || event.window.event == SDL_WINDOWEVENT_MINIMIZED))
                    this->renderer->ResizeSwapChain();
                else if ((event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) && ImGui::GetIO().WantCaptureKeyboard == false)
                    this->inputListener.injectKeyInput(event.key.keysym.sym, event.key.keysym.scancode, event.key.state, event.key.keysym.mod);
                else if ((event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) && ImGui::GetIO().WantCaptureMouse == false)
                    this->inputListener.injectMouseButtonInput(event.button.button, event.button.state, 0);
                else if (event.type == SDL_MOUSEMOTION)
                    this->inputListener.injectMouseMoveInput(event.motion.x, event.motion.y);
            }

            this->inputListener.process();

            if (SDL_GetWindowFlags(window) & SDL_WINDOW_MOUSE_FOCUS)
            {
                if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT))
                    SDL_WarpMouseInWindow(window, (int)(1920.0f * 0.5f), (int)(1080.0f * 0.5f));
            }

            // Start the Dear ImGui frame
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplSDL2_NewFrame(window);
            ImGui::NewFrame();

            // Gameplay
            scene->update(dt);

            DEBUG_LAYER::DebugLayer::GetInstance()->Draw();

            // Rendering
            ImGui::Render();

            if (this->renderer->AcquireNextImageIndex() == true)
            {
                std::vector<GAME::CameraComponent*> cameras = scene->getRoot()->GetActor()->getAllComponent<GAME::CameraComponent>();

                size_t cameraCount = cameras.size();
                for (size_t i = 0; i < cameraCount; ++i)
                {
                    cameras[i]->render(*scene);
                }

                this->renderer->SwapBuffer();
            }
        }

        return true;
    }

    InputListener* Application::GetInputListenner()
    {
        return &this->inputListener;
    }
}

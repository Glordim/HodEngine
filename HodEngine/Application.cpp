#include "Application.h"

#include <SDL.h>

#include "GraphicsSettings.h"

#include "Renderer.h"
#include "RendererVulkan.h"
#include "RendererDirectX12.h"

#include "Scene.hpp"
#include "CameraComponent.h"

Application::Application()
{
    this->window = nullptr;
    this->renderer = nullptr;
    this->pxFoundation = nullptr;
    this->pxPhysics = nullptr;
}

Application::~Application()
{
    if (this->window != nullptr)
    {
        SDL_DestroyWindow(this->window);
    }

    if (SDL_WasInit(0) != 0)
    {
        SDL_Quit();
    }

    if (this->pxPhysics != nullptr)
        this->pxPhysics->release();

    if (this->pxFoundation != nullptr)
        this->pxFoundation->release();
}

bool Application::Init()
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        fprintf(stderr, "SDL: Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    this->pxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, this->defaultAllocator, this->defaultErrorCallback);
    if (this->pxFoundation == nullptr)
        return false;

    physx::PxTolerancesScale tolerancesScale;

    this->pxPhysics = PxCreateBasePhysics(PX_PHYSICS_VERSION, *this->pxFoundation, tolerancesScale);
    if (this->pxPhysics == nullptr)
        return false;

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

bool Application::Run(Scene* scene)
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
            //if (TwEventSDL(&event, SDL_MAJOR_VERSION, SDL_MINOR_VERSION) != 0)
            //    continue;

            if (event.type == SDL_QUIT)
                shouldExit = true;
            else if (event.type == SDL_WINDOWEVENT && (event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED || event.window.event == SDL_WINDOWEVENT_HIDDEN || event.window.event == SDL_WINDOWEVENT_MINIMIZED))
                this->renderer->ResizeSwapChain();
            else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP)
                this->inputListener.injectKeyInput(event.key.keysym.sym, event.key.keysym.scancode, event.key.state, event.key.keysym.mod);
            else if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP)
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

        // Gameplay
        scene->update(dt);

        /*
        // TODO move in actor::update or component
        // Rotate Gun
        {
            SceneComponent* sceneComponent = gun->getComponent<SceneComponent>();
            //sceneComponent->rotate(glm::radians(25.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        {
            SceneComponent* sceneComponent = gun2->getComponent<SceneComponent>();
            //sceneComponent->rotate(glm::radians(25.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // TODO move in actor::update or component
        // Move light
        {
            SceneComponent* sceneComponent = light->getComponent<SceneComponent>();
            sceneComponent->setPosition(sceneComponent->getPosition() + glm::vec3(0.0f, 0.0035f, 0.0f) * sin((float)time));
        }
        {
            SceneComponent* sceneComponent = light2->getComponent<SceneComponent>();
            sceneComponent->setPosition(sceneComponent->getPosition() + glm::vec3(0.0f, 0.0035f, 0.0f) * sin((float)time));
        }
        */

        if (this->renderer->AcquireNextImageIndex() == true)
        {
            std::vector<CameraComponent*> cameras = scene->getRoot()->getActor()->getAllComponent<CameraComponent>();

            size_t cameraCount = cameras.size();
            for (size_t i = 0; i < cameraCount; ++i)
            {
                cameras[i]->render(*scene);
            }

            this->renderer->SwapBuffer();
        }

        

        //scene->drawDebugPhysics(cameraComponent, dt);

        //TwDraw();

        
    }

    return true;
}


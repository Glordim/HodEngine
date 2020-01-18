#pragma once

#include <String>
#include "InputListener.h"

#include <PxPhysicsAPI.h>

struct SDL_Window;

namespace HOD
{
    struct GraphicsSettings;

    class Renderer;

    namespace GAME
    {
        class Scene;
    }

    class Application
    {
    public:
        Application();
        virtual ~Application();

        bool Init();
        bool CreateWindowAndContext(const std::string& name, const GraphicsSettings& graphicsSettings);
        bool Run(GAME::Scene* scene);

        InputListener* GetInputListenner();

    private:
        SDL_Window* window;
        Renderer* renderer;
        InputListener inputListener;

        physx::PxDefaultAllocator defaultAllocator;
        physx::PxDefaultErrorCallback defaultErrorCallback;

        physx::PxFoundation* pxFoundation;
        physx::PxPhysics* pxPhysics;
    };
}

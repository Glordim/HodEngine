#pragma once

namespace HOD
{
    namespace GAME
    {
        class SceneComponent;
        class Scene;
    }

    class RenderQueue;

    class RenderQueueHelper
    {
    public:
        static void AddSceneComponent(RenderQueue& renderQueue, GAME::SceneComponent* sceneComponent, bool recursive);
        static void AddScenePhysicsDebug(RenderQueue& renderQueue, GAME::Scene* scene);
    };
}

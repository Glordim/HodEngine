#ifndef __RENDER_QUEUE_HELPER_HPP__
#define __RENDER_QUEUE_HELPER_HPP__

class RenderQueue;
class SceneComponent;

class RenderQueueHelper
{
public:
    static void AddSceneComponent(RenderQueue& renderQueue, SceneComponent* sceneComponent, bool recursive);
};

#endif

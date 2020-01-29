#pragma once

#include <vector>

#include "../Component.h"

namespace HOD
{
    namespace GAME
    {
        class CameraComponent;
        class PointLightComponent;

        class RendererComponent : public Component
        {
        public:
            RendererComponent(Actor* actor);
            RendererComponent(const RendererComponent& copy) = delete;
            ~RendererComponent() override = default;

            void        DrawImGui() override;
            const char* GetName() override;
        };
    }
}

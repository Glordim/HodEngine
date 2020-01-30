#pragma once

#include <Core/src/Color.h>

#include "../../Component.h"

#include <Renderer/src/Light/PointLight.h>

namespace HOD
{
    namespace GAME
    {
        class PointLightComponent : public Component
        {
        public:
                            PointLightComponent(Actor* actor);
                            PointLightComponent(const PointLightComponent& copy) = delete;
                            ~PointLightComponent() override = default;

            void            DrawImGui() override;
            const char*     GetName() override;

            PointLight*     GetPointLight() const;

        public:
            PointLight      _data;
        };
    }
}

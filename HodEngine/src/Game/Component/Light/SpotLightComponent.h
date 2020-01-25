#pragma once

#include <Core/src/Color.h>

#include "../../Component.h"

#include "../../../Renderer/Light/SpotLight.h"

namespace HOD
{
    namespace GAME
    {
        class SpotLightComponent : public Component
        {
        public:
            SpotLightComponent(Actor* actor);
            SpotLightComponent(const SpotLightComponent& copy) = delete;
            ~SpotLightComponent() override = default;

            void            DrawImGui() override;
            const char*     GetName() override;

            SpotLight*      GetSpotLight() const;

        public:

            SpotLight       _data;
        };
    }
}

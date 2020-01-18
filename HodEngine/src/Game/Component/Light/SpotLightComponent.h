#pragma once

#include "../Component.h"
#include "../../../Color.h"

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

            void            setupTweakBar(TwBar* bar) override;

            SpotLight*      GetSpotLight() const;

        public:

            SpotLight       _data;
        };
    }
}

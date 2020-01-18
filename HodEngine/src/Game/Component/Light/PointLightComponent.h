#pragma once

#include "../Component.h"
#include "../../../Color.h"

#include "../../../Renderer/Light/PointLight.h"

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

            void            setupTweakBar(TwBar* bar) override;

            PointLight*     GetPointLight() const;

        public:
            PointLight      _data;
        };
    }
}

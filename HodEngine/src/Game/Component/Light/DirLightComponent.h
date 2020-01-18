#pragma once

#include "../Component.h"
#include "../../../Color.h"

#include "../../../Renderer/Light/DirLight.h"

namespace HOD
{
    namespace GAME
    {
        class DirLightComponent : public Component
        {
        public:
            DirLightComponent(Actor* actor);
            DirLightComponent(const DirLightComponent& copy) = delete;
            ~DirLightComponent() override = default;

            void        setupTweakBar(TwBar* bar) override;

            DirLight*   GetDirLight() const;

        public:
            DirLight    _data;
        };
    }
}

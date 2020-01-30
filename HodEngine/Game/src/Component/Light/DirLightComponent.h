#pragma once

#include <Core/src/Color.h>

#include "../../Component.h"

#include <Renderer/src/Light/DirLight.h>

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

            void        DrawImGui() override;
            const char* GetName() override;

            DirLight*   GetDirLight() const;

        public:
            DirLight    _data;
        };
    }
}

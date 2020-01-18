#pragma once

#include "Component.h"

namespace HOD
{
    namespace GAME
    {
        class ColliderComponent : public Component
        {
        public:
            enum Shape
            {
                Box,
                Sphere,
                Mesh
            };

        public:
            ColliderComponent(Actor* actor);
            ColliderComponent(const ColliderComponent&) = delete;
            ~ColliderComponent() override = default;

            void    setupTweakBar(TwBar* bar) override;

            void SetShape(Shape shape);
        };
    }
}

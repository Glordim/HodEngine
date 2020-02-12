#pragma once

#include "../Component.h"

namespace HOD
{
    namespace PHYSICS
    {
        enum SHAPE;
    }

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

            void        DrawImGui() override;
            const char* GetName() override;

            void SetShape(PHYSICS::SHAPE eShape);
        };
    }
}

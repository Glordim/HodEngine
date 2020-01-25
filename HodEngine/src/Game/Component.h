#pragma once

namespace HOD
{
    namespace GAME
    {
        class Actor;
        
        class Component
        {
            friend class Actor;

        public:
            Actor*              GetActor() const;

            virtual void        DrawImGui() = 0;
            virtual const char* GetName() = 0;

        protected:
                                Component(Actor* actor);
                                Component(const Component&) = delete;
                                Component(const Component&&) = delete;
            virtual             ~Component() = default;

        private:
            Actor*              _actor = nullptr;
        };
    }
}

#include "Component.inl"

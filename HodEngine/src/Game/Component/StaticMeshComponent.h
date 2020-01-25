#pragma once

#include "RendererComponent.h"

namespace HOD
{
    class Mesh;
    class MaterialInstance;

    namespace GAME
    {
        class StaticMeshComponent : public RendererComponent
        {
        public:
            StaticMeshComponent(Actor* actor);
            StaticMeshComponent(const StaticMeshComponent& copy) = delete;
            ~StaticMeshComponent() override;

            void        DrawImGui() override;
            const char* GetName() override;

        public:
            void setMesh(Mesh* mesh);
            Mesh* GetMesh() const;

            void setMaterialInstance(MaterialInstance* materialInstance);
            MaterialInstance* GetMaterialInstance() const;

            void EnableDebugTangent(bool enable);
            bool IsDebugTangentEnabled() const;

        private:
            Mesh* mesh;
            MaterialInstance* materialInstance;

            bool debugTangent;
        };
    }
}

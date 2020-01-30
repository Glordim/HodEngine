#include "StaticMeshComponent.h"

#include <Renderer/src/Material.h>
#include <Renderer/src/Mesh.h>

#include "../Actor.h"

namespace HOD
{
    namespace GAME
    {
        StaticMeshComponent::StaticMeshComponent(Actor* actor) : RendererComponent(actor)
        {
            this->debugTangent = false;
        }

        StaticMeshComponent::~StaticMeshComponent()
        {

        }

        void StaticMeshComponent::DrawImGui()
        {

        }

        const char* StaticMeshComponent::GetName()
        {
            return "Static Mesh";
        }

        void StaticMeshComponent::setMesh(Mesh* mesh)
        {
            this->mesh = mesh;
        }

        Mesh* StaticMeshComponent::GetMesh() const
        {
            return this->mesh;
        }

        void StaticMeshComponent::setMaterialInstance(MaterialInstance* materialInstance)
        {
            this->materialInstance = materialInstance;
        }

        MaterialInstance* StaticMeshComponent::GetMaterialInstance() const
        {
            return this->materialInstance;
        }

        void StaticMeshComponent::EnableDebugTangent(bool enable)
        {
            this->debugTangent = enable;
        }

        bool StaticMeshComponent::IsDebugTangentEnabled() const
        {
            return this->debugTangent;
        }
    }
}

#ifndef __STATIC_MESH_COMPONENT_HPP__
#define __STATIC_MESH_COMPONENT_HPP__

#include "RendererComponent.h"

#include "Mesh.h"
#include "Material.h"

class StaticMeshComponent : public RendererComponent
{
public:
    StaticMeshComponent() = delete;
    StaticMeshComponent(const StaticMeshComponent& copy) = delete;

    StaticMeshComponent(Actor* actor);
    virtual ~StaticMeshComponent();

    friend class Actor;

public:
    virtual void draw(CameraComponent* camera, std::vector<LightComponent*>& allLight) override;

    void setMesh(Mesh* mesh);
    void setMaterial(Material* material);

private:
    Mesh* mesh;
    Material* material;
};

#endif

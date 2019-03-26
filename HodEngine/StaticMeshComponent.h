#ifndef __STATIC_MESH_COMPONENT_HPP__
#define __STATIC_MESH_COMPONENT_HPP__

#include "RendererComponent.h"

class Mesh;
class Material;

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

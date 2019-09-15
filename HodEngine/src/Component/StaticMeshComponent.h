#ifndef __STATIC_MESH_COMPONENT_HPP__
#define __STATIC_MESH_COMPONENT_HPP__

#include "RendererComponent.h"

class Mesh;
class MaterialInstance;

class StaticMeshComponent : public RendererComponent
{
public:
    StaticMeshComponent() = delete;
    StaticMeshComponent(const StaticMeshComponent& copy) = delete;

    StaticMeshComponent(Actor* actor);
    virtual ~StaticMeshComponent();

    friend class Actor;

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

#endif

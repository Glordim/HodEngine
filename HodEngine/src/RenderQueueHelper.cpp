#include "RenderQueueHelper.h"

#include "Renderer/RenderQueue.h"
#include "Component/SceneComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Component/LightComponent.h"
#include "Actor.h"
#include "Scene.hpp"

void RenderQueueHelper::AddSceneComponent(RenderQueue& renderQueue, SceneComponent* sceneComponent, bool recursive)
{
    size_t childCount = sceneComponent->getChildCount();
    for (size_t i = 0; i < childCount; ++i)
    {
        SceneComponent* child = sceneComponent->getChild(i);

        if (recursive == true)
            RenderQueueHelper::AddSceneComponent(renderQueue, child, recursive);

        StaticMeshComponent* staticMeshComponent = child->getActor()->getComponent<StaticMeshComponent>();
        if (staticMeshComponent != nullptr)
        {
            renderQueue.AddMesh(staticMeshComponent->GetMesh(), staticMeshComponent->GetMaterialInstance(), child->getModelMatrix());
        }

        LightComponent* lightComponent = child->getActor()->getComponent<LightComponent>();
        if (lightComponent != nullptr)
        {
            renderQueue.AddPointLight(child->getPosition(), lightComponent->GetPointLight());
        }
    }
}

void RenderQueueHelper::AddScenePhysicsDebug(RenderQueue& renderQueue, Scene* scene)
{
    physx::PxScene* pxScene = scene->GetPxScene();

    const physx::PxRenderBuffer& rb = pxScene->getRenderBuffer();
    physx::PxU32 lineCount = rb.getNbLines();

    if (lineCount != 0)
    {
        std::vector<Line_3P_3C> lines;
        lines.resize(lineCount);

        for (physx::PxU32 i = 0; i < lineCount; i++)
        {
            const physx::PxDebugLine& pxLine = rb.getLines()[i];

            Line_3P_3C& line = lines[i];

            line.vertices[0].pos[0] = pxLine.pos0.x;
            line.vertices[0].pos[1] = pxLine.pos0.y;
            line.vertices[0].pos[2] = pxLine.pos0.z;
            line.vertices[0].color[0] = (pxLine.color0 & 0x00FF0000) >> 16;
            line.vertices[0].color[1] = (pxLine.color0 & 0x0000FF00) >> 8;
            line.vertices[0].color[2] = (pxLine.color0 & 0x000000FF) >> 0;

            line.vertices[1].pos[0] = pxLine.pos1.x;
            line.vertices[1].pos[1] = pxLine.pos1.y;
            line.vertices[1].pos[2] = pxLine.pos1.z;
            line.vertices[1].color[0] = (pxLine.color1 & 0x00FF0000) >> 16;
            line.vertices[1].color[1] = (pxLine.color1 & 0x0000FF00) >> 8;
            line.vertices[1].color[2] = (pxLine.color1 & 0x000000FF) >> 0;
        }

        renderQueue.AddLines(lines, nullptr, glm::identity<glm::mat4x4>());
    }

    physx::PxU32 triCount = rb.getNbTriangles();

    if (triCount != 0)
    {
        std::vector<Tri_3P_3C> tris;
        tris.resize(triCount);

        for (physx::PxU32 i = 0; i < triCount; i++)
        {
            const physx::PxDebugTriangle& pxTri = rb.getTriangles()[i];
            // render the line

            Tri_3P_3C& tri = tris[i];

            tri.vertices[0].pos[0] = pxTri.pos0.x;
            tri.vertices[0].pos[1] = pxTri.pos0.y;
            tri.vertices[0].pos[2] = pxTri.pos0.z;
            tri.vertices[0].color[0] = (pxTri.color0 & 0x00FF0000) >> 16;
            tri.vertices[0].color[1] = (pxTri.color0 & 0x0000FF00) >> 8;
            tri.vertices[0].color[2] = (pxTri.color0 & 0x000000FF) >> 0;

            tri.vertices[1].pos[0] = pxTri.pos1.x;
            tri.vertices[1].pos[1] = pxTri.pos1.y;
            tri.vertices[1].pos[2] = pxTri.pos1.z;
            tri.vertices[1].color[0] = (pxTri.color1 & 0x00FF0000) >> 16;
            tri.vertices[1].color[1] = (pxTri.color1 & 0x0000FF00) >> 8;
            tri.vertices[1].color[2] = (pxTri.color1 & 0x000000FF) >> 0;

            tri.vertices[2].pos[0] = pxTri.pos2.x;
            tri.vertices[2].pos[1] = pxTri.pos2.y;
            tri.vertices[2].pos[2] = pxTri.pos2.z;
            tri.vertices[2].color[0] = (pxTri.color2 & 0x00FF0000) >> 16;
            tri.vertices[2].color[1] = (pxTri.color2 & 0x0000FF00) >> 8;
            tri.vertices[2].color[2] = (pxTri.color2 & 0x000000FF) >> 0;
        }

        renderQueue.AddTriangles(tris, nullptr, glm::identity<glm::mat4x4>());
    }
}

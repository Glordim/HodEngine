#include "RenderQueueHelper.h"

#include <Renderer/src/RenderQueue.h>
#include "Component/SceneComponent.h"
#include "Component/StaticMeshComponent.h"
#include "Component/Light/DirLightComponent.h"
#include "Component/Light/PointLightComponent.h"
#include "Component/Light/SpotLightComponent.h"
#include "Actor.h"
#include "Scene.h"

#include <Physic/src/Scene.h>

namespace HOD
{
    void RenderQueueHelper::AddSceneComponent(RenderQueue& renderQueue, GAME::SceneComponent* sceneComponent, bool recursive)
    {
        size_t childCount = sceneComponent->getChildCount();
        for (size_t i = 0; i < childCount; ++i)
        {
            GAME::SceneComponent* child = sceneComponent->getChild(i);

            if (recursive == true)
                RenderQueueHelper::AddSceneComponent(renderQueue, child, recursive);

            GAME::StaticMeshComponent* staticMeshComponent = child->GetActor()->getComponent<GAME::StaticMeshComponent>();
            if (staticMeshComponent != nullptr)
            {
                renderQueue.AddMesh(staticMeshComponent->GetMesh(), staticMeshComponent->GetMaterialInstance(), child->getModelMatrix());

                if (staticMeshComponent->IsDebugTangentEnabled() == true)
                {
                    const std::vector<Vertex_3P_3C_3N_2UV_3TA>& vertices = staticMeshComponent->GetMesh()->GetVertices();

                    size_t verticesCount = vertices.size();

                    std::vector<Line_3P_3C> tangentLines;
                    tangentLines.reserve(verticesCount);

                    float red[3] = { 1.0f, 0.0f, 0.0f };
                    float green[3] = { 0.0f, 1.0f, 0.0f };
                    float blue[3] = { 0.0f, 0.0f, 1.0f };

                    for (size_t vertexIndex = 0; vertexIndex < verticesCount; ++vertexIndex)
                    {
                        const Vertex_3P_3C_3N_2UV_3TA& vertex = vertices[vertexIndex];

                        Line_3P_3C lineX;
                        memcpy(lineX.vertices[0].pos, vertex.pos, sizeof(vertex.pos));
                        memcpy(lineX.vertices[1].pos, vertex.pos, sizeof(vertex.pos));
                        lineX.vertices[1].pos[0] += vertex.tangent[0] * 0.1f;
                        lineX.vertices[1].pos[1] += vertex.tangent[1] * 0.1f;
                        lineX.vertices[1].pos[2] += vertex.tangent[2] * 0.1f;

                        memcpy(lineX.vertices[0].color, red, sizeof(red));
                        memcpy(lineX.vertices[1].color, red, sizeof(red));

                        Line_3P_3C lineY;
                        memcpy(lineY.vertices[0].pos, vertex.pos, sizeof(vertex.pos));
                        memcpy(lineY.vertices[1].pos, vertex.pos, sizeof(vertex.pos));
                        lineY.vertices[1].pos[0] += vertex.bitangent[0] * 0.1f;
                        lineY.vertices[1].pos[1] += vertex.bitangent[1] * 0.1f;
                        lineY.vertices[1].pos[2] += vertex.bitangent[2] * 0.1f;

                        memcpy(lineY.vertices[0].color, green, sizeof(green));
                        memcpy(lineY.vertices[1].color, green, sizeof(green));

                        Line_3P_3C lineZ;
                        memcpy(lineZ.vertices[0].pos, vertex.pos, sizeof(vertex.pos));
                        memcpy(lineZ.vertices[1].pos, vertex.pos, sizeof(vertex.pos));
                        lineZ.vertices[1].pos[0] += vertex.normal[0] * 0.1f;
                        lineZ.vertices[1].pos[1] += vertex.normal[1] * 0.1f;
                        lineZ.vertices[1].pos[2] += vertex.normal[2] * 0.1f;

                        memcpy(lineZ.vertices[0].color, blue, sizeof(blue));
                        memcpy(lineZ.vertices[1].color, blue, sizeof(blue));

                        tangentLines.push_back(std::move(lineX));
                        tangentLines.push_back(std::move(lineY));
                        tangentLines.push_back(std::move(lineZ));
                    }

                    renderQueue.AddLines(tangentLines, nullptr, child->getModelMatrix());
                }
            }

            GAME::DirLightComponent* dirLightComponent = child->GetActor()->getComponent<GAME::DirLightComponent>();
            if (dirLightComponent != nullptr)
            {
                renderQueue.AddDirLight(child->getRotationEuler(), dirLightComponent->GetDirLight());
            }

            GAME::PointLightComponent* pointLightComponent = child->GetActor()->getComponent<GAME::PointLightComponent>();
            if (pointLightComponent != nullptr)
            {
                renderQueue.AddPointLight(child->getPosition(), pointLightComponent->GetPointLight());
            }

            GAME::SpotLightComponent* spotLightComponent = child->GetActor()->getComponent<GAME::SpotLightComponent>();
            if (spotLightComponent != nullptr)
            {
                renderQueue.AddSpotLight(child->getPosition(), child->getRotation() * glm::vec3(0.0f, 0.0f, 1.0f), spotLightComponent->GetSpotLight());
            }
        }
    }

    void RenderQueueHelper::AddScenePhysicsDebug(RenderQueue& renderQueue, GAME::Scene* scene)
    {
		std::vector<Line_3P_3C> lines;
		std::vector<Tri_3P_3C> tris;
		scene->GetPhysicScene()->GetDebugGeometry(lines, tris);

        if (lines.empty() == false)
        {
            renderQueue.AddLines(lines, nullptr, glm::identity<glm::mat4x4>());
        }
        if (tris.empty() == false)
        {
            renderQueue.AddTriangles(tris, nullptr, glm::identity<glm::mat4x4>());
        }
    }

	void RenderQueueHelper::AddDebugLines(RenderQueue& renderQueue, GAME::Scene* scene)
	{
		std::vector<Line_3P_3C> lines;

		scene->GetDebugLines(lines);

		if (lines.empty() == false)
		{
			renderQueue.AddLines(lines, nullptr, glm::identity<glm::mat4x4>());
		}
	}
}

#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/RenderQueueHelper.hpp"

/*
#include "HodEngine/Game/Component/SceneComponent.hpp"
#include "HodEngine/Game/Component/Light/DirLightComponent.hpp"
#include "HodEngine/Game/Component/Light/PointLightComponent.hpp"
#include "HodEngine/Game/Component/Light/SpotLightComponent.hpp"
*/
#include "HodEngine/Game/Scene.hpp"

#include <HodEngine/Renderer/RenderQueue.hpp>

namespace hod
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void RenderQueueHelper::AddSceneComponent(renderer::RenderQueue& renderQueue, game::SceneComponent* sceneComponent, bool recursive)
	{
		/*
		size_t childCount = sceneComponent->GetChildCount();
		for (size_t i = 0; i < childCount; ++i)
		{
			game::SceneComponent* child = sceneComponent->GetChild(i);

			if (recursive == true)
			{
				RenderQueueHelper::AddSceneComponent(renderQueue, child, recursive);
			}

			game::StaticMeshComponent* staticMeshComponent = child->GetActor()->GetComponent<game::StaticMeshComponent>();
			if (staticMeshComponent != nullptr)
			{
				renderQueue.AddMesh(staticMeshComponent->GetMesh(), staticMeshComponent->GetMaterialInstance(), child->GetModelMatrix());

				if (staticMeshComponent->IsDebugTangentEnabled() == true)
				{
					const Vector<renderer::Vertex_3P_3C_3N_2UV_3TA>& vertices = staticMeshComponent->GetMesh()->GetVertices();

					size_t verticesCount = vertices.size();

					Vector<renderer::Line_3P_3C> tangentLines;
					tangentLines.reserve(verticesCount);

					float red[3] = { 1.0f, 0.0f, 0.0f };
					float green[3] = { 0.0f, 1.0f, 0.0f };
					float blue[3] = { 0.0f, 0.0f, 1.0f };

					for (size_t vertexIndex = 0; vertexIndex < verticesCount; ++vertexIndex)
					{
						const renderer::Vertex_3P_3C_3N_2UV_3TA& vertex = vertices[vertexIndex];

						renderer::Line_3P_3C lineX;
						memcpy(lineX.vertices[0].pos, vertex.pos, sizeof(vertex.pos));
						memcpy(lineX.vertices[1].pos, vertex.pos, sizeof(vertex.pos));
						lineX.vertices[1].pos[0] += vertex.tangent[0] * 0.1f;
						lineX.vertices[1].pos[1] += vertex.tangent[1] * 0.1f;
						lineX.vertices[1].pos[2] += vertex.tangent[2] * 0.1f;

						memcpy(lineX.vertices[0].color, red, sizeof(red));
						memcpy(lineX.vertices[1].color, red, sizeof(red));

						renderer::Line_3P_3C lineY;
						memcpy(lineY.vertices[0].pos, vertex.pos, sizeof(vertex.pos));
						memcpy(lineY.vertices[1].pos, vertex.pos, sizeof(vertex.pos));
						lineY.vertices[1].pos[0] += vertex.bitangent[0] * 0.1f;
						lineY.vertices[1].pos[1] += vertex.bitangent[1] * 0.1f;
						lineY.vertices[1].pos[2] += vertex.bitangent[2] * 0.1f;

						memcpy(lineY.vertices[0].color, green, sizeof(green));
						memcpy(lineY.vertices[1].color, green, sizeof(green));

						renderer::Line_3P_3C lineZ;
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

					renderQueue.AddLines(tangentLines, nullptr, child->GetModelMatrix());
				}
			}

			game::DirLightComponent* dirLightComponent = child->GetActor()->GetComponent<game::DirLightComponent>();
			if (dirLightComponent != nullptr)
			{
				renderQueue.AddDirLight(child->GetRotationEuler(), dirLightComponent->GetDirLight());
			}

			game::PointLightComponent* pointLightComponent = child->GetActor()->GetComponent<game::PointLightComponent>();
			if (pointLightComponent != nullptr)
			{
				renderQueue.AddPointLight(child->GetPosition(), pointLightComponent->GetPointLight());
			}

			game::SpotLightComponent* spotLightComponent = child->GetActor()->GetComponent<game::SpotLightComponent>();
			if (spotLightComponent != nullptr)
			{
				renderQueue.AddSpotLight(child->GetPosition(), child->GetRotation() * glm::vec3(0.0f, 0.0f, 1.0f), spotLightComponent->GetSpotLight());
			}
		}
		*/
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void RenderQueueHelper::AddScenePhysicsDebug(renderer::RenderQueue& renderQueue, game::Scene* scene)
	{
		/*
		Vector<renderer::Line_3P_3C> lines;
		Vector<renderer::Tri_3P_3C> tris;
		scene->GetPhysicScene()->GetDebugGeometry(lines, tris);

		if (lines.empty() == false)
		{
			renderQueue.AddLines(lines, nullptr, Matrix4::Identity);
		}
		if (tris.empty() == false)
		{
			renderQueue.AddTriangles(tris, nullptr, Matrix4::Identity);
		}
		*/
	}

	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	void RenderQueueHelper::AddDebugLines(renderer::RenderQueue& renderQueue, game::Scene* scene)
	{
		/*
		Vector<renderer::Line_3P_3C> lines;

		scene->GetDebugLines(lines);

		if (lines.empty() == false)
		{
			renderQueue.AddLines(lines, nullptr, Matrix4::Identity);
		}
		*/
	}
}

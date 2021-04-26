
#include "DebugDrawer.h"

#include "Renderer/src/P2fC4f.h"

#include "Renderer/src/Renderer.h"
#include "Renderer/src/RenderCommand/RenderCommandMesh.h"

#include "Renderer/src/MaterialManager.h"

#include "glm/gtc/matrix_transform.hpp"

namespace HOD
{
	namespace PHYSICS
	{
		RENDERER::MaterialInstance* DebugDrawer::_pointMaterialInstance = nullptr;
		RENDERER::MaterialInstance* DebugDrawer::_lineMaterialInstance = nullptr;
		RENDERER::MaterialInstance* DebugDrawer::_wireframePolygonMaterialInstance = nullptr;
		RENDERER::MaterialInstance* DebugDrawer::_solidPolygonMaterialInstance = nullptr;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DebugDrawer::DebugDrawer()
		{
			if (DebugDrawer::_solidPolygonMaterialInstance == nullptr)
			{
				RENDERER::MaterialManager* materialManager = RENDERER::MaterialManager::GetInstance();
				RENDERER::Material* material = materialManager->GetData(materialManager->CreateMaterial("UnlitColor"));
				_solidPolygonMaterialInstance = RENDERER::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
			if (DebugDrawer::_wireframePolygonMaterialInstance == nullptr)
			{
				RENDERER::MaterialManager* materialManager = RENDERER::MaterialManager::GetInstance();
				RENDERER::Material* material = materialManager->GetData(materialManager->CreateMaterial("UnlitColor", RENDERER::Material::Topololy::TRIANGLE_LINE));
				_wireframePolygonMaterialInstance = RENDERER::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		DebugDrawer::~DebugDrawer()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::PurgePreviousFrame()
		{
			for (void* pointer : pointerToDelete)
			{
				delete[] pointer;
			}
			pointerToDelete.clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawPolygon(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = new RENDERER::P2fC4f[vertexCount];
			for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				vertices[vertexIndex]._position[0] = b2Vertices[vertexIndex].x;
				vertices[vertexIndex]._position[1] = b2Vertices[vertexIndex].x;
				vertices[vertexIndex]._color[0] = color.r;
				vertices[vertexIndex]._color[1] = color.g;
				vertices[vertexIndex]._color[2] = color.b;
				vertices[vertexIndex]._color[3] = color.a;
			}

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			RENDERER::RenderCommandMesh* renderCommand = new RENDERER::RenderCommandMesh(vertices, vertexCount, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_solidPolygonMaterialInstance, true);

			RENDERER::RenderQueue* renderQueue = RENDERER::Renderer::GetInstance()->GetRenderQueue();
			renderQueue->PushRenderCommand(renderCommand);

			pointerToDelete.push_back(vertices);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawSolidPolygon(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = new RENDERER::P2fC4f[vertexCount];
			for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				vertices[vertexIndex]._position[0] = b2Vertices[vertexIndex].x;
				vertices[vertexIndex]._position[1] = b2Vertices[vertexIndex].x;
				vertices[vertexIndex]._color[0] = color.r;
				vertices[vertexIndex]._color[1] = color.g;
				vertices[vertexIndex]._color[2] = color.b;
				vertices[vertexIndex]._color[3] = color.a;
			}

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			RENDERER::RenderCommandMesh* renderCommand = new RENDERER::RenderCommandMesh(vertices, vertexCount, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_solidPolygonMaterialInstance, true);

			RENDERER::RenderQueue* renderQueue = RENDERER::Renderer::GetInstance()->GetRenderQueue();
			renderQueue->PushRenderCommand(renderCommand);

			pointerToDelete.push_back(vertices);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
		{
			// Todo
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
		{
			// Todo
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
		{
			// Todo
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawTransform(const b2Transform& xf)
		{
			// Todo

			//RENDERER::RenderQueue* renderQueue = RENDERER::Renderer::GetInstance()->GetRenderQueue();
			//renderQueue->PushRenderCommand();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
		{
			//RENDERER::RenderQueue* renderQueue = RENDERER::Renderer::GetInstance()->GetRenderQueue();
			//renderQueue->PushRenderCommand();
		}
	}
}

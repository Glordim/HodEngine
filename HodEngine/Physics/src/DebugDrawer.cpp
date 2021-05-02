
#include "DebugDrawer.h"

#include "Renderer/src/P2fC4f.h"

#include "Renderer/src/Renderer.h"
#include "Renderer/src/RenderCommand/RenderCommandMesh.h"

#include "Renderer/src/MaterialManager.h"

#include "glm/gtc/matrix_transform.hpp"

#define M_PI 3.14159265358979323846
#define DEGREE_TO_RADIAN (M_PI / 180.0f)

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
				RENDERER::Material* material = materialManager->GetData(materialManager->CreateMaterial("UnlitColor", RENDERER::Material::PolygonMode::Fill, RENDERER::Material::Topololy::TRIANGLE_FAN));
				_solidPolygonMaterialInstance = RENDERER::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
			if (DebugDrawer::_wireframePolygonMaterialInstance == nullptr)
			{
				RENDERER::MaterialManager* materialManager = RENDERER::MaterialManager::GetInstance();
				RENDERER::Material* material = materialManager->GetData(materialManager->CreateMaterial("UnlitColor", RENDERER::Material::PolygonMode::Line, RENDERER::Material::Topololy::TRIANGLE_FAN));
				_wireframePolygonMaterialInstance = RENDERER::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
			if (DebugDrawer::_lineMaterialInstance == nullptr)
			{
				RENDERER::MaterialManager* materialManager = RENDERER::MaterialManager::GetInstance();
				RENDERER::Material* material = materialManager->GetData(materialManager->CreateMaterial("UnlitColor", RENDERER::Material::PolygonMode::Line, RENDERER::Material::Topololy::LINE));
				_lineMaterialInstance = RENDERER::Renderer::GetInstance()->CreateMaterialInstance(material);
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
		void DebugDrawer::PushToRenderQueue(RENDERER::RenderQueue& renderQueue)
		{
			for (RENDERER::RenderCommand* renderCommand : _renderCommands)
			{
				renderQueue.PushRenderCommand(renderCommand);
			}

			_renderCommands.clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool DebugDrawer::HasRenderCommand() const
		{
			return (_renderCommands.empty() == false);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawPolygon(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = BuildPolygonVertices(b2Vertices, vertexCount, color);

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			_renderCommands.push_back(new RENDERER::RenderCommandMesh(vertices, vertexCount, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_wireframePolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawSolidPolygon(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = BuildPolygonVertices(b2Vertices, vertexCount, color);

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			_renderCommands.push_back(new RENDERER::RenderCommandMesh(vertices, vertexCount, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_solidPolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RENDERER::P2fC4f* DebugDrawer::BuildPolygonVertices(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = new RENDERER::P2fC4f[vertexCount];

			for (uint32_t vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				vertices[vertexIndex]._position[0] = b2Vertices[vertexIndex].x;
				vertices[vertexIndex]._position[1] = b2Vertices[vertexIndex].y;
				vertices[vertexIndex]._color[0] = color.r;
				vertices[vertexIndex]._color[1] = color.g;
				vertices[vertexIndex]._color[2] = color.b;
				vertices[vertexIndex]._color[3] = 0.1f;
			}

			return vertices;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = BuildCircleVertices(center, radius, color, 32);

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			_renderCommands.push_back(new RENDERER::RenderCommandMesh(vertices, 32 * 3, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_wireframePolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = BuildCircleVertices(center, radius, color, 32);

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			_renderCommands.push_back(new RENDERER::RenderCommandMesh(vertices, 32 * 3, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_solidPolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		RENDERER::P2fC4f* DebugDrawer::BuildCircleVertices(const b2Vec2& center, float radius, const b2Color& color, uint32_t segmentCount)
		{
			const float angleStep = 360.0f / segmentCount;

			RENDERER::P2fC4f* vertices = new RENDERER::P2fC4f[segmentCount * 3];

			for (uint32_t currentSegment = 0; currentSegment < segmentCount; ++currentSegment)
			{
				uint32_t offset = currentSegment * 3;

				vertices[offset + 0]._position[0] = center.x + 0.0f;
				vertices[offset + 0]._position[1] = center.y + 0.0f;
				vertices[offset + 0]._color[0] = color.r;
				vertices[offset + 0]._color[1] = color.g;
				vertices[offset + 0]._color[2] = color.b;
				vertices[offset + 0]._color[3] = 0.1f;

				float angle = angleStep * currentSegment;
				angle *= DEGREE_TO_RADIAN;

				vertices[offset + 1]._position[0] = center.x + (radius * cos(angle));
				vertices[offset + 1]._position[1] = center.y + (radius * sin(angle));
				vertices[offset + 1]._color[0] = color.r;
				vertices[offset + 1]._color[1] = color.g;
				vertices[offset + 1]._color[2] = color.b;
				vertices[offset + 1]._color[3] = 0.1f;

				angle = angleStep * (currentSegment + 1);
				angle *= DEGREE_TO_RADIAN;

				vertices[offset + 2]._position[0] = center.x + (radius * cos(angle));
				vertices[offset + 2]._position[1] = center.y + (radius * sin(angle));
				vertices[offset + 2]._color[0] = color.r;
				vertices[offset + 2]._color[1] = color.g;
				vertices[offset + 2]._color[2] = color.b;
				vertices[offset + 2]._color[3] = 0.1f;
			}

			return vertices;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
		{
			RENDERER::P2fC4f* vertices = new RENDERER::P2fC4f[2];

			vertices[0]._position[0] = p1.x;
			vertices[0]._position[1] = p1.y;
			vertices[0]._color[0] = color.r;
			vertices[0]._color[1] = color.g;
			vertices[0]._color[2] = color.b;
			vertices[0]._color[3] = 0.1f;

			vertices[1]._position[0] = p2.x;
			vertices[1]._position[1] = p2.y;
			vertices[1]._color[0] = color.r;
			vertices[1]._color[1] = color.g;
			vertices[1]._color[2] = color.b;
			vertices[1]._color[3] = 0.1f;

			glm::mat4 modelMatrix = glm::identity<glm::mat4>();
			_renderCommands.push_back(new RENDERER::RenderCommandMesh(vertices, 2, sizeof(RENDERER::P2fC4f), nullptr, 0, modelMatrix, DebugDrawer::_lineMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawTransform(const b2Transform& xf)
		{
			// Todo
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void DebugDrawer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
		{
			// Todo
		}
	}
}

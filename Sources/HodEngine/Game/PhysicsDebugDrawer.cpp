
#include "PhysicsDebugDrawer.hpp"

#include "HodEngine/Renderer/P2fC4f.hpp"

#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Core/Math/Matrix4.hpp"
#include "HodEngine/Core/Math/Math.hpp"

namespace hod
{
	namespace game
	{
		renderer::MaterialInstance* PhysicsDebugDrawer::_pointMaterialInstance = nullptr;
		renderer::MaterialInstance* PhysicsDebugDrawer::_lineMaterialInstance = nullptr;
		renderer::MaterialInstance* PhysicsDebugDrawer::_wireframePolygonMaterialInstance = nullptr;
		renderer::MaterialInstance* PhysicsDebugDrawer::_solidPolygonMaterialInstance = nullptr;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PhysicsDebugDrawer::PhysicsDebugDrawer()
		{
			renderer::MaterialManager* materialManager = renderer::MaterialManager::GetInstance();

			if (PhysicsDebugDrawer::_solidPolygonMaterialInstance == nullptr)
			{
				const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Fill_TriangleFan);
				_solidPolygonMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
			if (PhysicsDebugDrawer::_wireframePolygonMaterialInstance == nullptr)
			{
				const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_TriangleFan);
				_wireframePolygonMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
			if (PhysicsDebugDrawer::_lineMaterialInstance == nullptr)
			{
				const renderer::Material* material = materialManager->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fC4f_Unlit_Line_Line);
				_lineMaterialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
			}
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PhysicsDebugDrawer::~PhysicsDebugDrawer()
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			for (renderer::RenderCommand* renderCommand : _renderCommands)
			{
				renderQueue.PushRenderCommand(renderCommand);
			}

			_renderCommands.clear();
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		bool PhysicsDebugDrawer::HasRenderCommand() const
		{
			return (_renderCommands.empty() == false);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawPolygon(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			renderer::P2fC4f* vertices = BuildPolygonVertices(b2Vertices, vertexCount, color);

			_renderCommands.push_back(new renderer::RenderCommandMesh(vertices, vertexCount, sizeof(renderer::P2fC4f), nullptr, 0, Matrix4::Identity, PhysicsDebugDrawer::_wireframePolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawSolidPolygon(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			renderer::P2fC4f* vertices = BuildPolygonVertices(b2Vertices, vertexCount, color);

			_renderCommands.push_back(new renderer::RenderCommandMesh(vertices, vertexCount, sizeof(renderer::P2fC4f), nullptr, 0, Matrix4::Identity, PhysicsDebugDrawer::_solidPolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::P2fC4f* PhysicsDebugDrawer::BuildPolygonVertices(const b2Vec2* b2Vertices, int32 vertexCount, const b2Color& color)
		{
			renderer::P2fC4f* vertices = new renderer::P2fC4f[vertexCount];

			for (int32 vertexIndex = 0; vertexIndex < vertexCount; ++vertexIndex)
			{
				vertices[vertexIndex]._position[0] = b2Vertices[vertexIndex].x;
				vertices[vertexIndex]._position[1] = b2Vertices[vertexIndex].y;
				vertices[vertexIndex]._color[0] = color.r;
				vertices[vertexIndex]._color[1] = color.g;
				vertices[vertexIndex]._color[2] = color.b;
				vertices[vertexIndex]._color[3] = 0.5f;
			}

			return vertices;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawCircle(const b2Vec2& center, float radius, const b2Color& color)
		{
			renderer::P2fC4f* vertices = BuildCircleVertices(center, radius, color, 32);

			_renderCommands.push_back(new renderer::RenderCommandMesh(vertices, 32 * 3, sizeof(renderer::P2fC4f), nullptr, 0, Matrix4::Identity, PhysicsDebugDrawer::_wireframePolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color)
		{
			renderer::P2fC4f* vertices = BuildCircleVertices(center, radius, color, 32);

			_renderCommands.push_back(new renderer::RenderCommandMesh(vertices, 32 * 3, sizeof(renderer::P2fC4f), nullptr, 0, Matrix4::Identity, PhysicsDebugDrawer::_solidPolygonMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		renderer::P2fC4f* PhysicsDebugDrawer::BuildCircleVertices(const b2Vec2& center, float radius, const b2Color& color, uint32_t segmentCount)
		{
			const float angleStep = 360.0f / segmentCount;

			renderer::P2fC4f* vertices = new renderer::P2fC4f[segmentCount * 3];

			for (uint32_t currentSegment = 0; currentSegment < segmentCount; ++currentSegment)
			{
				uint32_t offset = currentSegment * 3;

				vertices[offset + 0]._position[0] = center.x + 0.0f;
				vertices[offset + 0]._position[1] = center.y + 0.0f;
				vertices[offset + 0]._color[0] = color.r;
				vertices[offset + 0]._color[1] = color.g;
				vertices[offset + 0]._color[2] = color.b;
				vertices[offset + 0]._color[3] = 0.5f;

				float angle = angleStep * currentSegment;
				angle = math::DegreeToRadian(angle);

				vertices[offset + 1]._position[0] = center.x + (radius * cos(angle));
				vertices[offset + 1]._position[1] = center.y + (radius * sin(angle));
				vertices[offset + 1]._color[0] = color.r;
				vertices[offset + 1]._color[1] = color.g;
				vertices[offset + 1]._color[2] = color.b;
				vertices[offset + 1]._color[3] = 0.5f;

				angle = angleStep * (currentSegment + 1);
				angle = math::DegreeToRadian(angle);

				vertices[offset + 2]._position[0] = center.x + (radius * cos(angle));
				vertices[offset + 2]._position[1] = center.y + (radius * sin(angle));
				vertices[offset + 2]._color[0] = color.r;
				vertices[offset + 2]._color[1] = color.g;
				vertices[offset + 2]._color[2] = color.b;
				vertices[offset + 2]._color[3] = 0.5f;
			}

			return vertices;
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color)
		{
			renderer::P2fC4f* vertices = new renderer::P2fC4f[2];

			vertices[0]._position[0] = p1.x;
			vertices[0]._position[1] = p1.y;
			vertices[0]._color[0] = color.r;
			vertices[0]._color[1] = color.g;
			vertices[0]._color[2] = color.b;
			vertices[0]._color[3] = 0.5f;

			vertices[1]._position[0] = p2.x;
			vertices[1]._position[1] = p2.y;
			vertices[1]._color[0] = color.r;
			vertices[1]._color[1] = color.g;
			vertices[1]._color[2] = color.b;
			vertices[1]._color[3] = 0.5f;

			_renderCommands.push_back(new renderer::RenderCommandMesh(vertices, 2, sizeof(renderer::P2fC4f), nullptr, 0, Matrix4::Identity, PhysicsDebugDrawer::_lineMaterialInstance, true));
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawTransform(const b2Transform& xf)
		{
			// Todo
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void PhysicsDebugDrawer::DrawPoint(const b2Vec2& p, float size, const b2Color& color)
		{
			// Todo
		}
	}
}

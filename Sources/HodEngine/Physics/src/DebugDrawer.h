#pragma once

#include <box2d/b2_draw.h>
#include <vector>

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;
		class RenderQueue;
		class RenderCommand;
		struct P2fC4f;
	}

	namespace physics
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class DebugDrawer : public b2Draw
		{
		public:
													DebugDrawer();
													~DebugDrawer();

		public:

			void									PushToRenderQueue(renderer::RenderQueue& renderQueue);
			bool									HasRenderCommand() const;

			void									DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
			void									DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

			void									DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
			void									DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

			void									DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
			void									DrawTransform(const b2Transform& xf) override;
			void									DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

		private:

			static renderer::P2fC4f*				BuildPolygonVertices(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
			static renderer::P2fC4f*				BuildCircleVertices(const b2Vec2& center, float radius, const b2Color& color, uint32_t segmentCount);

		private:

			std::vector<renderer::RenderCommand*>	_renderCommands;

		private:

			static renderer::MaterialInstance*		_pointMaterialInstance;
			static renderer::MaterialInstance*		_lineMaterialInstance;
			static renderer::MaterialInstance*		_wireframePolygonMaterialInstance;
			static renderer::MaterialInstance*		_solidPolygonMaterialInstance;
		};
	}
}

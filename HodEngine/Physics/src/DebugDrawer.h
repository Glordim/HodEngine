#pragma once

#include <box2d/b2_draw.h>
#include <vector>

namespace HOD
{
	namespace RENDERER
	{
		class MaterialInstance;
		class RenderQueue;
		class RenderCommand;
		struct P2fC4f;
	}

	namespace PHYSICS
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

			void									PushToRenderQueue(RENDERER::RenderQueue& renderQueue);
			bool									HasRenderCommand() const;

			void									DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
			void									DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;

			void									DrawCircle(const b2Vec2& center, float radius, const b2Color& color) override;
			void									DrawSolidCircle(const b2Vec2& center, float radius, const b2Vec2& axis, const b2Color& color) override;

			void									DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
			void									DrawTransform(const b2Transform& xf) override;
			void									DrawPoint(const b2Vec2& p, float size, const b2Color& color) override;

		private:

			static RENDERER::P2fC4f*				BuildPolygonVertices(const b2Vec2* vertices, int32 vertexCount, const b2Color& color);
			static RENDERER::P2fC4f*				BuildCircleVertices(const b2Vec2& center, float radius, const b2Color& color, uint32_t segmentCount);

		private:

			std::vector<RENDERER::RenderCommand*>	_renderCommands;

		private:

			static RENDERER::MaterialInstance*		_pointMaterialInstance;
			static RENDERER::MaterialInstance*		_lineMaterialInstance;
			static RENDERER::MaterialInstance*		_wireframePolygonMaterialInstance;
			static RENDERER::MaterialInstance*		_solidPolygonMaterialInstance;
		};
	}
}
#pragma once

#include <cstdint>

#define GLM_DEPTH_ZERO_TO_ONE 1
#define GLM_FORCE_LEFT_HANDED 1
#include "glm/mat4x4.hpp"

#include <HodEngine/Core/Src/Rect.h>
#include <vector>

namespace HOD
{
	namespace RENDERER
	{
		class MaterialInstance;
		class Buffer;
		class Context;

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class CommandBuffer
		{
		public:

								CommandBuffer() = default;
								CommandBuffer(const CommandBuffer&) = delete;
								CommandBuffer(CommandBuffer&&) = delete;
			virtual				~CommandBuffer();

			void				operator=(const CommandBuffer&) = delete;
			void				operator=(CommandBuffer&&) = delete;

		public:

			void				PurgePointerToDelete();
			void				DeleteAfterRender(void* pointer);

			virtual bool		StartRecord(Context* context = nullptr) = 0;
			virtual bool		EndRecord() = 0;

			virtual void		SetProjectionMatrix(const glm::mat4x4& projectionMatrix) = 0;
			virtual void		SetViewMatrix(const glm::mat4x4& viewMatrix) = 0;
			virtual void		SetModelMatrix(const glm::mat4x4& modelMatrix) = 0;

			virtual void		SetViewport(const CORE::Rect& viewport) = 0;
			virtual void		SetScissor(const CORE::Rect& scissor) = 0;

			virtual void		SetMaterialInstance(MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) = 0;
			virtual void		SetVertexBuffer(Buffer* vertexBuffer, uint32_t offset = 0) = 0;
			virtual void		SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) = 0;

			virtual void		Draw(uint32_t vertexCount) = 0;
			virtual void		DrawIndexed(uint32_t indexCount) = 0;

		private:

			std::vector<void*>	pointerToDelete;
		};
	}
}

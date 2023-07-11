#pragma once

#include <cstdint>

#include <HodEngine/Core/Rect.h>
#include <vector>

#include "HodEngine/Renderer/RHI/Shader.h"

namespace hod
{
	class Matrix4;

	namespace renderer
	{
		class MaterialInstance;
		class Buffer;
		class Context;
		class RenderTarget;

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
			void				DeleteAfterRender(MaterialInstance* materialInstance);
			void				DeleteAfterRender(Buffer* buffer);

			virtual bool		StartRecord(RenderTarget* renderTarget = nullptr, Context* context = nullptr) = 0;
			virtual bool		EndRecord() = 0;

			virtual void		SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) = 0;

			virtual void		SetProjectionMatrix(const Matrix4& projectionMatrix) = 0;
			virtual void		SetViewMatrix(const Matrix4& viewMatrix) = 0;
			virtual void		SetModelMatrix(const Matrix4& modelMatrix) = 0;

			virtual void		SetViewport(const Rect& viewport) = 0;
			virtual void		SetScissor(const Rect& scissor) = 0;

			virtual void		SetMaterialInstance(MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) = 0;
			virtual void		SetVertexBuffer(Buffer* vertexBuffer, uint32_t offset = 0) = 0;
			virtual void		SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) = 0;

			virtual void		Draw(uint32_t vertexCount) = 0;
			virtual void		DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

		private:

			std::vector<MaterialInstance*>	_materialInstanceToDelete;
			std::vector<Buffer*>			_bufferToDelete;
		};
	}
}

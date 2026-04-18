#pragma once
#include "HodEngine/Renderer/Export.hpp"

#include <cstdint>

#include "HodEngine/Core/Vector.hpp"

#include "HodEngine/Math/Color.hpp"
#include "HodEngine/Math/Matrix4.hpp"
#include "HodEngine/Renderer/RHI/Shader.hpp"

namespace hod::inline math
{
	struct Rect;
}

namespace hod::renderer
{
	class Material;
	class MaterialInstance;
	class Buffer;
	class PresentationSurface;
	class RenderTarget;

	/// @brief
	class HOD_RENDERER_API CommandBuffer
	{
	public:
		CommandBuffer() = default;
		CommandBuffer(const CommandBuffer&) = delete;
		CommandBuffer(CommandBuffer&&) = delete;
		virtual ~CommandBuffer();

		void operator=(const CommandBuffer&) = delete;
		void operator=(CommandBuffer&&) = delete;

	public:
		void PurgePointerToDelete();
		void DeleteAfterRender(MaterialInstance* materialInstance);
		void DeleteAfterRender(Buffer* buffer);

		virtual bool StartRecord() = 0;
		virtual bool EndRecord() = 0;

		virtual bool StartRenderPass(RenderTarget* renderTarget = nullptr, PresentationSurface* presentationSurface = nullptr,
		                             const math::Color& color = math::Color(0.1f, 0.1f, 0.1f, 1.0f)) = 0;
		virtual bool EndRenderPass() = 0;

		virtual void SetConstant(void* constant, uint32_t size, Shader::ShaderType shaderType) = 0;

		virtual void SetProjectionMatrix(const math::Matrix4& projectionMatrix) = 0;
		virtual void SetViewMatrix(const math::Matrix4& viewMatrix) = 0;
		virtual void SetModelMatrix(const math::Matrix4& modelMatrix) = 0;

		virtual void SetViewport(const math::Rect& viewport) = 0;
		virtual void SetScissor(const math::Rect& scissor) = 0;

		virtual void SetMaterial(const Material* material) = 0;
		virtual void SetMaterialInstance(const MaterialInstance* materialInstance, uint32_t setOffset = 2, uint32_t setCount = UINT32_MAX) = 0;
		virtual void SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset = 0) = 0;
		virtual void SetIndexBuffer(Buffer* indexBuffer, uint32_t offset = 0) = 0;

		virtual void Draw(uint32_t vertexCount) = 0;
		virtual void DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset) = 0;

		virtual void Present(PresentationSurface* presentationSurface) = 0;

		// TODO
		math::Matrix4 _projection;
		math::Matrix4 _view;

	private:
		Vector<MaterialInstance*> _materialInstanceToDelete;
		Vector<Buffer*>           _bufferToDelete;
	};
}

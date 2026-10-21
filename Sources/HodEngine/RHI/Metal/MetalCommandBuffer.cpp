#include "HodEngine/RHI/Pch.hpp"
#include "HodEngine/RHI/Metal/MetalBindGroup.hpp"
#include "HodEngine/RHI/Metal/MetalBuffer.hpp"
#include "HodEngine/RHI/Metal/MetalCommandBuffer.hpp"

#include "HodEngine/RHI/Metal/MetalGraphicsPipeline.hpp"
#include "HodEngine/RHI/Metal/MetalPresentationSurface.hpp"
#include "HodEngine/RHI/Metal/MetalTexture.hpp"
#include "HodEngine/RHI/Metal/MetalRhiDevice.hpp"
#include "HodEngine/RHI/RenderTarget.hpp"

#include <HodEngine/Core/Assert.hpp>
#include <HodEngine/Core/Output/OutputService.hpp>
#include <HodEngine/Math/Rect.hpp>

#include <Metal/Metal.hpp>

#include <QuartzCore/CAMetalDrawable.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include <HodEngine/Core/Math.hpp>
#include <cstring>

namespace hod::inline rhi
{
	namespace
	{
		/// @brief Metal has no point fill mode: the triangles of a pipeline in PolygonMode::Point are drawn as their vertices, as on Vulkan.
		/// Nor triangle fan: drawn as a triangle list, see IsDrawnAsTriangleList.
		MTL::PrimitiveType ToMetalPrimitiveType(const MetalGraphicsPipeline& graphicsPipeline)
		{
			switch (graphicsPipeline.GetTopology())
			{
				case GraphicsPipeline::Topology::POINT: return MTL::PrimitiveTypePoint;
				case GraphicsPipeline::Topology::LINE: return MTL::PrimitiveTypeLine;
				case GraphicsPipeline::Topology::LINE_STRIP: return MTL::PrimitiveTypeLineStrip;
				default: return graphicsPipeline.GetPolygonMode() == GraphicsPipeline::PolygonMode::Point ? MTL::PrimitiveTypePoint : MTL::PrimitiveTypeTriangle;
			}
		}

		/// @brief Whether the triangle fan of the pipeline is drawn as a triangle list, its indices made by the draw
		bool IsDrawnAsTriangleList(const MetalGraphicsPipeline& graphicsPipeline)
		{
			return graphicsPipeline.GetTopology() == GraphicsPipeline::Topology::TRIANGLE_FAN && ToMetalPrimitiveType(graphicsPipeline) == MTL::PrimitiveTypeTriangle;
		}
	}

	/// @brief
	MetalCommandBuffer::MetalCommandBuffer()
	{
		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();
		MTL::Device*   device = metalRhiDevice->GetDevice();

		_frameIndex = metalRhiDevice->GetFrameIndex();

		_commandBuffer = device->newCommandBuffer();
		_commandBuffer->beginCommandBuffer(metalRhiDevice->GetCommandAllocator(_frameIndex));

		MTL4::ArgumentTableDescriptor* argumentTableDescriptor = MTL4::ArgumentTableDescriptor::alloc()->init();
		argumentTableDescriptor->setMaxBufferBindCount(31);
		argumentTableDescriptor->setMaxTextureBindCount(31);
		argumentTableDescriptor->setMaxSamplerStateBindCount(16);
		NS::Error*                     error = nullptr;
		_vertexArgumentTable = device->newArgumentTable(argumentTableDescriptor, &error);
		_fragmentArgumentTable = device->newArgumentTable(argumentTableDescriptor, &error);
		argumentTableDescriptor->release();
	}

	/// @brief
	MetalCommandBuffer::~MetalCommandBuffer()
	{
		if (_renderCommandEncoder)
		{
			_renderCommandEncoder->release();
		}
		// Deleted once its submission is done (see RenderView::Wait), but not always: what it holds is kept until the GPU is done with it
		MetalRhiDevice* metalRhiDevice = MetalRhiDevice::GetInstance();
		metalRhiDevice->DeferRelease(_vertexArgumentTable);
		metalRhiDevice->DeferRelease(_fragmentArgumentTable);
		metalRhiDevice->DeferRelease(_commandBuffer);
	}

	/// @brief From the transient memory of its frame, reused once the GPU is done with it
	/// @param size
	/// @param gpuAddress
	/// @return
	void* MetalCommandBuffer::AllocateTransient(uint32_t size, uint64_t& gpuAddress)
	{
		return MetalRhiDevice::GetInstance()->AllocateTransient(_frameIndex, size, gpuAddress);
	}

	/// @brief
	/// @return
	uint32_t MetalCommandBuffer::GetFrameIndex() const
	{
		return _frameIndex;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::StartRecord()
	{
		return true;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::EndRecord()
	{
		_commandBuffer->endCommandBuffer();
		return true;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::StartRenderPass(RenderTarget* renderTarget, PresentationSurface* presentationSurface, const Color& color)
	{
		MTL::Texture* colorTexture = nullptr;
		MTL::Texture* depthTexture = nullptr;

		if (renderTarget != nullptr)
		{
			// Either of them can be missing, see RenderTarget::Init
			if (renderTarget->GetColorTexture() != nullptr)
			{
				colorTexture = static_cast<MetalTexture*>(renderTarget->GetColorTexture())->GetNativeTexture();
			}
			if (renderTarget->GetDepthTexture() != nullptr)
			{
				depthTexture = static_cast<MetalTexture*>(renderTarget->GetDepthTexture())->GetNativeTexture();
			}
			SetRenderPassFormats(renderTarget->GetColorFormat(), renderTarget->GetDepthFormat());

			Vector2 resolution = renderTarget->GetResolution();
			_renderPassWidth = (uint32_t)resolution.GetX();
			_renderPassHeight = (uint32_t)resolution.GetY();
		}
		else
		{
			MetalPresentationSurface* metalPresentationSurface = static_cast<MetalPresentationSurface*>(presentationSurface);
			CA::MetalDrawable*        drawable = metalPresentationSurface->GetCurrentDrawable();
			colorTexture = drawable->texture();
			depthTexture = static_cast<MetalTexture*>(metalPresentationSurface->GetDepthTexture())->GetNativeTexture();
			SetRenderPassFormats(PresentationSurface::ColorFormat, PresentationSurface::DepthFormat);

			_renderPassWidth = (uint32_t)colorTexture->width();
			_renderPassHeight = (uint32_t)colorTexture->height();
		}

		MTL4::RenderPassDescriptor* renderPassDescriptor = MTL4::RenderPassDescriptor::alloc()->init();
		if (colorTexture != nullptr)
		{
			renderPassDescriptor->colorAttachments()->object(0)->setTexture(colorTexture);
			renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
			renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
			renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(color.r, color.g, color.b, color.a));
		}
		// Cleared to the farthest by each render pass, and not kept after it, as on the other backends
		if (depthTexture != nullptr)
		{
			renderPassDescriptor->depthAttachment()->setTexture(depthTexture);
			renderPassDescriptor->depthAttachment()->setLoadAction(MTL::LoadActionClear);
			renderPassDescriptor->depthAttachment()->setStoreAction(MTL::StoreActionDontCare);
			renderPassDescriptor->depthAttachment()->setClearDepth(1.0);
		}

		_renderCommandEncoder = _commandBuffer->renderCommandEncoder(renderPassDescriptor);
		renderPassDescriptor->release();

		// Metal 4 does not track the hazards between passes: what the work committed before draws or reads (a render target sampled here,
		// or sampled before and drawn into here) is done, and visible, before this pass reads or draws. As the subpass dependencies of Vulkan.
		constexpr MTL::Stages renderStages = static_cast<MTL::Stages>(MTL::StageVertex | MTL::StageFragment);
		_renderCommandEncoder->barrierAfterQueueStages(renderStages, renderStages, MTL4::VisibilityOptionDevice);

		_renderCommandEncoder->setArgumentTable(_vertexArgumentTable, MTL::RenderStageVertex);
		_renderCommandEncoder->setArgumentTable(_fragmentArgumentTable, MTL::RenderStageFragment);

		return true;
	}

	/// @brief
	/// @return
	bool MetalCommandBuffer::EndRenderPass()
	{
		_renderCommandEncoder->endEncoding();

		return true;
	}

	/// @brief
	/// @param constant
	/// @param Size
	/// @param shaderType
	void MetalCommandBuffer::SetConstant(void* constant, uint32_t Size, Shader::ShaderType /*shaderType*/)
	{
		uint64_t gpuAddress = 0;
		memcpy(AllocateTransient(Size, gpuAddress), constant, Size);

		// The constant is the first buffer of both stages
		_vertexArgumentTable->setAddress(gpuAddress, 0);
		_fragmentArgumentTable->setAddress(gpuAddress, 0);
	}

	/// @brief
	/// @param viewport
	void MetalCommandBuffer::SetViewport(const Rect& viewport)
	{
		MTL::Viewport mtlViewport;
		mtlViewport.originX = viewport._position.GetX();
		mtlViewport.originY = viewport._position.GetY();
		mtlViewport.width = viewport._size.GetX();
		mtlViewport.height = viewport._size.GetY();
		mtlViewport.znear = 0;
		mtlViewport.zfar = 1;
		_renderCommandEncoder->setViewport(mtlViewport);
	}

	/// @brief
	/// @param scissor
	void MetalCommandBuffer::SetScissor(const Rect& scissor)
	{
		MTL::ScissorRect scissorRect;
		scissorRect.x = Clamp((uint32_t)scissor._position.GetX(), 0u, _renderPassWidth);
		scissorRect.y = Clamp((uint32_t)scissor._position.GetY(), 0u, _renderPassHeight);
		scissorRect.width = Clamp((uint32_t)scissor._size.GetX(), 0u, _renderPassWidth - (uint32_t)scissorRect.x);
		scissorRect.height = Clamp((uint32_t)scissor._size.GetY(), 0u, _renderPassHeight - (uint32_t)scissorRect.y);
		_renderCommandEncoder->setScissorRect(scissorRect);
	}

	void MetalCommandBuffer::SetGraphicsPipeline(const GraphicsPipeline* graphicsPipeline)
	{
		if (IsCompatible(*graphicsPipeline) == false)
		{
			return;
		}

		_graphicsPipeline = static_cast<const MetalGraphicsPipeline*>(graphicsPipeline);
		_renderCommandEncoder->setRenderPipelineState(_graphicsPipeline->GetNativeRenderPipeline());
		_renderCommandEncoder->setDepthStencilState(_graphicsPipeline->GetNativeDepthStencilState());

		// Culling is not part of the pipeline state on Metal
		MTL::CullMode cullMode = MTL::CullModeNone;
		if (_graphicsPipeline->GetCullMode() == CullMode::Back)
		{
			cullMode = MTL::CullModeBack;
		}
		else if (_graphicsPipeline->GetCullMode() == CullMode::Front)
		{
			cullMode = MTL::CullModeFront;
		}
		_renderCommandEncoder->setCullMode(cullMode);
		_renderCommandEncoder->setFrontFacingWinding(MTL::WindingCounterClockwise);

		// Nor the fill mode, which only applies to triangles
		_renderCommandEncoder->setTriangleFillMode(_graphicsPipeline->GetPolygonMode() == GraphicsPipeline::PolygonMode::Line ? MTL::TriangleFillModeLines
		                                                                                                                    : MTL::TriangleFillModeFill);
	}

	/// @brief
	void MetalCommandBuffer::SetBindGroup(uint32_t /*set*/, const BindGroup* bindGroup, const uint32_t* uniformBufferOffsets, uint32_t uniformBufferOffsetCount)
	{
		const MetalBindGroup* metalBindGroup = static_cast<const MetalBindGroup*>(bindGroup);
		Assert(uniformBufferOffsetCount == metalBindGroup->GetUniformBufferCount());
		(void)uniformBufferOffsetCount;

		uint32_t argumentBufferSize = metalBindGroup->GetArgumentBufferSize();
		if (argumentBufferSize == 0)
		{
			return;
		}

		// The offsets are part of the argument buffer: each binding gets its own
		uint64_t gpuAddress = 0;
		metalBindGroup->WriteArgumentBuffer(AllocateTransient(argumentBufferSize, gpuAddress), uniformBufferOffsets);

		_vertexArgumentTable->setAddress(gpuAddress, metalBindGroup->GetBufferIndex());
		_fragmentArgumentTable->setAddress(gpuAddress, metalBindGroup->GetBufferIndex());
	}

	/// @brief
	/// @param vertexBuffer
	/// @param count
	/// @param offset
	void MetalCommandBuffer::SetVertexBuffer(Buffer** vertexBuffer, uint32_t count, uint32_t offset)
	{
		NS::Range range = _graphicsPipeline->GetVertexAttributeBufferRange();

		for (uint32_t index = 0; index < count; ++index)
		{
			MTL::Buffer* mtlBuffer = static_cast<MetalBuffer*>(vertexBuffer[index])->GetNativeBuffer();
			_vertexArgumentTable->setAddress(mtlBuffer->gpuAddress() + offset, range.location + index);
		}
	}

	/// @brief
	/// @param indexBuffer
	/// @param offset
	void MetalCommandBuffer::SetIndexBuffer(Buffer* indexBuffer, uint32_t offset)
	{
		_indexBuffer = static_cast<MetalBuffer*>(indexBuffer);
		_indexBufferOffset = offset;
	}

	/// @brief
	/// @param vertexCount
	void MetalCommandBuffer::Draw(uint32_t vertexCount)
	{
		if (IsDrawnAsTriangleList(*_graphicsPipeline))
		{
			if (vertexCount < 3)
			{
				return;
			}

			// The fan (0, 1, 2, 3...) is the triangles (0, 1, 2), (0, 2, 3)...
			uint32_t  indexCount = (vertexCount - 2) * 3;
			uint64_t  indexBufferAddress = 0;
			uint32_t* indices =static_cast<uint32_t*>(AllocateTransient(indexCount * sizeof(uint32_t), indexBufferAddress));
			for (uint32_t triangle = 0; triangle < vertexCount - 2; ++triangle)
			{
				indices[triangle * 3 + 0] = 0;
				indices[triangle * 3 + 1] = triangle + 1;
				indices[triangle * 3 + 2] = triangle + 2;
			}
			_renderCommandEncoder->drawIndexedPrimitives(MTL::PrimitiveTypeTriangle, indexCount, MTL::IndexTypeUInt32, indexBufferAddress, indexCount * sizeof(uint32_t));
			return;
		}

		_renderCommandEncoder->drawPrimitives(ToMetalPrimitiveType(*_graphicsPipeline), 0, vertexCount, 1);
	}

	/// @brief
	/// @param indexCount
	/// @param indexOffset
	/// @param vertexOffset
	void MetalCommandBuffer::DrawIndexed(uint32_t indexCount, uint32_t indexOffset, uint32_t vertexOffset)
	{
		MTL::GPUAddress indexBufferAddress = _indexBuffer->GetNativeBuffer()->gpuAddress() + indexOffset * sizeof(uint16_t) + _indexBufferOffset;

		if (IsDrawnAsTriangleList(*_graphicsPipeline))
		{
			if (indexCount < 3)
			{
				return;
			}

			// Made from the indices of the fan as they are now: the index buffer is in memory shared with the CPU
			const uint16_t* fanIndices =
				reinterpret_cast<const uint16_t*>(static_cast<const uint8_t*>(_indexBuffer->GetNativeBuffer()->contents()) + _indexBufferOffset) + indexOffset;

			uint32_t  triangleListIndexCount = (indexCount - 2) * 3;
			uint64_t  triangleListAddress = 0;
			uint16_t* indices = static_cast<uint16_t*>(AllocateTransient(triangleListIndexCount * sizeof(uint16_t), triangleListAddress));
			for (uint32_t triangle = 0; triangle < indexCount - 2; ++triangle)
			{
				indices[triangle * 3 + 0] = fanIndices[0];
				indices[triangle * 3 + 1] = fanIndices[triangle + 1];
				indices[triangle * 3 + 2] = fanIndices[triangle + 2];
			}

			indexCount = triangleListIndexCount;
			indexBufferAddress = triangleListAddress;
		}

		_renderCommandEncoder->drawIndexedPrimitives(ToMetalPrimitiveType(*_graphicsPipeline), indexCount, MTL::IndexTypeUInt16, indexBufferAddress, indexCount * sizeof(uint16_t), 1,
		                                             static_cast<NS::Integer>(vertexOffset), 0);
	}

	/// @brief Nothing to record: presented by MetalPresentationSurface::SwapBuffer, once the command buffers drawing into it are committed
	/// @param presentationSurface
	void MetalCommandBuffer::Present(PresentationSurface* /*presentationSurface*/) {}

	/// @brief
	/// @return
	MTL4::CommandBuffer* MetalCommandBuffer::GetNativeCommandBuffer() const
	{
		return _commandBuffer;
	}
}

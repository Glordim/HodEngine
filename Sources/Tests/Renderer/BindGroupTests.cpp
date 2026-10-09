#include <gtest/gtest.h>

#include "RenderTestHelpers.hpp"

#include "Shader/BindGroupTest_Fragment.hpp"
#include "Shader/BindGroupTest_Vertex.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector2.hpp>

#include <HodEngine/Renderer/Renderer.hpp>

#include <HodEngine/RHI/BindGroup.hpp>
#include <HodEngine/RHI/Buffer.hpp>
#include <HodEngine/RHI/CommandBuffer.hpp>
#include <HodEngine/RHI/Fence.hpp>
#include <HodEngine/RHI/GraphicsPipeline.hpp>
#include <HodEngine/RHI/LegacyMaterialInstance.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Shader.hpp>
#include <HodEngine/RHI/Texture.hpp>
#include <HodEngine/RHI/VertexInput.hpp>

#include <cstring>
#include <functional>

using namespace hod;

// ============================================================================
// Drives the RHI directly, with no Renderer object in between: a pipeline, its BindGroups, one command buffer.
//
// Shader/BindGroupTest.slang outputs image * ubo.color * global.tint, with
//   set 0: uniform block "global" (tint)
//   set 1: uniform block "ubo" (color), texture "image", sampler "imageSampler"
// ============================================================================

class BindGroupRender : public OutputCheckedTest
{
protected:
	void SetUp() override
	{
		OutputCheckedTest::SetUp();

		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		Texture::CreateInfo createInfo;
		createInfo._allowReadWrite = true;
		_renderTarget = rhiDevice->CreateRenderTarget();
		ASSERT_TRUE(_renderTarget->Init(DefaultTargetSize, DefaultTargetSize, createInfo));

		_vertexShader = rhiDevice->CreateShader(Shader::ShaderType::Vertex);
		ASSERT_TRUE(_vertexShader->LoadFromIR(BindGroupTest_Vertex, BindGroupTest_Vertex_size, BindGroupTest_Vertex_reflection, BindGroupTest_Vertex_reflection_size));

		_fragmentShader = rhiDevice->CreateShader(Shader::ShaderType::Fragment);
		ASSERT_TRUE(
			_fragmentShader->LoadFromIR(BindGroupTest_Fragment, BindGroupTest_Fragment_size, BindGroupTest_Fragment_reflection, BindGroupTest_Fragment_reflection_size));

		StaticArray<VertexInput, 2> vertexInputs = {
			VertexInput(0, 0, VertexInput::Format::R32G32_SFloat),
			VertexInput(1, 8, VertexInput::Format::R32G32_SFloat),
		};
		_graphicsPipeline = rhiDevice->CreateGraphicsPipeline(vertexInputs.Data(), (uint32_t)vertexInputs.Size(), _vertexShader, _fragmentShader);
		ASSERT_NE(_graphicsPipeline, nullptr);

		// Uniform blocks sharing a buffer start on a multiple of the device alignment
		_blockStride = rhiDevice->GetUniformBufferOffsetAlignment();
		while (_blockStride < sizeof(Vector4))
		{
			_blockStride *= 2;
		}
	}

	void TearDown() override
	{
		// Nothing drawn by the test may still be in flight when its resources are released
		RhiDevice::GetInstance()->WaitIdle();

		for (BindGroup* bindGroup : _bindGroups)
		{
			DefaultAllocator::GetInstance().Delete(bindGroup);
		}
		for (Buffer* buffer : _buffers)
		{
			DefaultAllocator::GetInstance().Delete(buffer);
		}
		for (Texture* texture : _textures)
		{
			DefaultAllocator::GetInstance().Delete(texture);
		}
		DefaultAllocator::GetInstance().Delete(_graphicsPipeline);
		DefaultAllocator::GetInstance().Delete(_vertexShader);
		DefaultAllocator::GetInstance().Delete(_fragmentShader);
		DefaultAllocator::GetInstance().Delete(_renderTarget);

		OutputCheckedTest::TearDown();
	}

	template<typename T>
	Buffer* CreateBuffer(Buffer::Usage usage, const T* data, uint32_t count)
	{
		return CreateBuffer(usage, data, count, sizeof(T));
	}

	// The elements are written every 'stride' bytes
	template<typename T>
	Buffer* CreateBuffer(Buffer::Usage usage, const T* data, uint32_t count, uint32_t stride)
	{
		Buffer* buffer = RhiDevice::GetInstance()->CreateBuffer(usage, count * stride);
		_buffers.PushBack(buffer);

		uint8_t* bufferData = static_cast<uint8_t*>(buffer->Lock());
		EXPECT_NE(bufferData, nullptr);
		if (bufferData != nullptr)
		{
			for (uint32_t index = 0; index < count; ++index)
			{
				std::memcpy(bufferData + index * stride, &data[index], sizeof(T));
			}
			buffer->Unlock();
		}
		return buffer;
	}

	// One uniform buffer holding the given colors, one block each, the block of color i starting at i * _blockStride
	Buffer* CreateColorBlocks(std::initializer_list<Color> colors)
	{
		Vector<Vector4> values;
		for (const Color& color : colors)
		{
			values.PushBack(ToVector4(color));
		}
		return CreateBuffer(Buffer::Usage::Uniform, values.Data(), (uint32_t)values.Size(), _blockStride);
	}

	BindGroup* CreateGlobalBindGroup(Buffer* tints)
	{
		BindGroup* bindGroup = RhiDevice::GetInstance()->CreateBindGroup(_graphicsPipeline, 0, &tints, 1, nullptr, 0);
		EXPECT_NE(bindGroup, nullptr);
		_bindGroups.PushBack(bindGroup);
		return bindGroup;
	}

	// The texture gives both the image and its sampler; null for the fallback texture
	BindGroup* CreateMaterialBindGroup(Buffer* colors, const Texture* texture)
	{
		StaticArray<const Texture*, 2> textures = {texture, texture}; // "image", "imageSampler"

		BindGroup* bindGroup = RhiDevice::GetInstance()->CreateBindGroup(_graphicsPipeline, 1, &colors, 1, textures.Data(), (uint32_t)textures.Size());
		EXPECT_NE(bindGroup, nullptr);
		_bindGroups.PushBack(bindGroup);
		return bindGroup;
	}

	// 2x2 texture, row by row from the first texel: Red, Green / Blue, Yellow
	Texture* CreateFourColorsTexture()
	{
		const uint8_t pixels[4 * 2 * 2] = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255};

		Texture::CreateInfo createInfo;
		createInfo._filterMode = FilterMode::Nearest;

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		EXPECT_TRUE(texture->BuildBuffer(2, 2, pixels, createInfo));
		_textures.PushBack(texture);
		return texture;
	}

	// Records one render pass on the target, submits it and blocks until it can be read back
	void RenderFrame(const std::function<void()>& draw)
	{
		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		Renderer::GetInstance()->AcquireNextFrame();

		// One texture per frame in flight: keep the one this frame renders to
		_frameTexture = _renderTarget->GetColorTexture();

		Rect viewport;
		viewport._position = Vector2(0.0f, 0.0f);
		viewport._size = Vector2((float)DefaultTargetSize, (float)DefaultTargetSize);

		_commandBuffer = rhiDevice->CreateCommandBuffer();
		ASSERT_TRUE(_commandBuffer->StartRecord());
		_commandBuffer->StartRenderPass(_renderTarget);
		_commandBuffer->SetViewport(viewport);
		_commandBuffer->SetGraphicsPipeline(_graphicsPipeline);

		Matrix4 mvp = Matrix4::Identity; // positions are given in clip space
		_commandBuffer->SetConstant(&mvp, sizeof(mvp), Shader::ShaderType::Vertex);

		draw();

		_commandBuffer->EndRenderPass();
		_frameTexture->CaptureReadback(_commandBuffer);
		_commandBuffer->EndRecord();

		Fence* fence = rhiDevice->CreateFence();
		fence->Reset();
		rhiDevice->SubmitCommandBuffers(&_commandBuffer, 1, nullptr, nullptr, fence);
		fence->Wait();

		DefaultAllocator::GetInstance().Delete(fence);
		DefaultAllocator::GetInstance().Delete(_commandBuffer);
		_commandBuffer = nullptr;

		Renderer::GetInstance()->Render();
	}

	void SetBindGroup(uint32_t set, const BindGroup* bindGroup, uint32_t blockIndex)
	{
		uint32_t offset = blockIndex * _blockStride;
		_commandBuffer->SetBindGroup(set, bindGroup, &offset, 1);
	}

	// Axis aligned quad, uv (0, 0) on its (left, bottom) corner
	void DrawQuad(float left, float bottom, float right, float top)
	{
		StaticArray<Vector2, 4> positions = {
			Vector2(left, top),
			Vector2(right, top),
			Vector2(left, bottom),
			Vector2(right, bottom),
		};

		StaticArray<Vector2, 4> uvs = {
			Vector2(0.0f, 1.0f),
			Vector2(1.0f, 1.0f),
			Vector2(0.0f, 0.0f),
			Vector2(1.0f, 0.0f),
		};

		StaticArray<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

		StaticArray<Buffer*, 2> vertexBuffers = {
			CreateBuffer(Buffer::Usage::Vertex, positions.Data(), (uint32_t)positions.Size()),
			CreateBuffer(Buffer::Usage::Vertex, uvs.Data(), (uint32_t)uvs.Size()),
		};

		_commandBuffer->SetVertexBuffer(vertexBuffers.Data(), (uint32_t)vertexBuffers.Size());
		_commandBuffer->SetIndexBuffer(CreateBuffer(Buffer::Usage::Index, indices.Data(), (uint32_t)indices.Size()));
		_commandBuffer->DrawIndexed((uint32_t)indices.Size(), 0, 0);
	}

	// Pixel of the last rendered frame; x and y in [-1, 1], same space as DrawQuad
	Color ReadPixel(float x, float y) const
	{
		// The viewport is flipped by the RHI so that +y goes up: row 0 is the top of the target
		float pixelX = (x * 0.5f + 0.5f) * (float)DefaultTargetSize;
		float pixelY = (0.5f - y * 0.5f) * (float)DefaultTargetSize;
		return _frameTexture->ReadPixel(Vector2(pixelX, pixelY));
	}

protected:
	RenderTarget*     _renderTarget = nullptr;
	Shader*           _vertexShader = nullptr;
	Shader*           _fragmentShader = nullptr;
	GraphicsPipeline* _graphicsPipeline = nullptr;
	uint32_t          _blockStride = 0;

	CommandBuffer* _commandBuffer = nullptr;
	Texture*       _frameTexture = nullptr;

	Vector<BindGroup*> _bindGroups;
	Vector<Buffer*>    _buffers;
	Vector<Texture*>   _textures;
};

// The same BindGroup serves several draws, each reading its own block of the uniform buffer
TEST_F(BindGroupRender, UniformBufferOffsetSelectsTheBlockOfEachDraw)
{
	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red, Green, Blue}), nullptr);

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);

		SetBindGroup(1, material, 0);
		DrawQuad(-1.0f, -1.0f, -0.5f, 1.0f);

		SetBindGroup(1, material, 1);
		DrawQuad(-0.5f, -1.0f, 0.0f, 1.0f);

		SetBindGroup(1, material, 2);
		DrawQuad(0.0f, -1.0f, 0.5f, 1.0f);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.25f, 0.0f), Green));
	EXPECT_TRUE(ColorNear(ReadPixel(0.25f, 0.0f), Blue));
	EXPECT_TRUE(ColorNear(ReadPixel(0.75f, 0.0f), Background));
}

// Each set keeps its own binding: changing the offset of one leaves the other alone
TEST_F(BindGroupRender, SetsAreBoundIndependently)
{
	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White, Red}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Yellow, White}), nullptr);

	RenderFrame([&]() {
		// Yellow * White
		SetBindGroup(0, global, 0);
		SetBindGroup(1, material, 0);
		DrawQuad(-1.0f, -1.0f, -0.5f, 1.0f);

		// Yellow * Red: only the global set changes
		SetBindGroup(0, global, 1);
		DrawQuad(-0.5f, -1.0f, 0.0f, 1.0f);

		// White * Red: only the material set changes
		SetBindGroup(1, material, 1);
		DrawQuad(0.0f, -1.0f, 0.5f, 1.0f);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, 0.0f), Yellow));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.25f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.25f, 0.0f), Red));
}

TEST_F(BindGroupRender, TexturesAreGivenPerBlock)
{
	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({White}), CreateFourColorsTexture());

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);
		SetBindGroup(1, material, 0);
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	});

	// uv (0, 0), the first texel, is on the (left, bottom) corner of the quad
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, -0.5f), Green));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.5f), Blue));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.5f), Yellow));
}

// Two BindGroups of the same set, differing by their texture, used in the same frame
TEST_F(BindGroupRender, BindGroupsOfTheSameSetCoexist)
{
	Buffer* colors = CreateColorBlocks({White});

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* textured = CreateMaterialBindGroup(colors, CreateFourColorsTexture());
	BindGroup* untextured = CreateMaterialBindGroup(colors, nullptr);

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);

		SetBindGroup(1, textured, 0);
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f);

		SetBindGroup(1, untextured, 0);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), White)); // the fallback texture is white for now
}

// A BindGroup is immutable, what a later frame sees is whatever its buffers hold then
TEST_F(BindGroupRender, BufferContentCanChangeBetweenFrames)
{
	Buffer* colors = CreateColorBlocks({Red});

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(colors, nullptr);

	auto draw = [&]() {
		SetBindGroup(0, global, 0);
		SetBindGroup(1, material, 0);
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	};

	RenderFrame(draw);
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));

	Vector4 green = ToVector4(Green);
	void*   data = colors->Lock();
	ASSERT_NE(data, nullptr);
	std::memcpy(data, &green, sizeof(green));
	colors->Unlock();

	RenderFrame(draw);
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

// The LegacyMaterialInstance path on a pipeline with several sets: each value must land in the set declaring it
TEST_F(BindGroupRender, MaterialInstanceSpansTheSetsOfThePipeline)
{
	LegacyMaterialInstance* materialInstance = RhiDevice::GetInstance()->CreateLegacyMaterialInstance(_graphicsPipeline);
	ASSERT_NE(materialInstance, nullptr);
	materialInstance->SetVec4("global.tint", ToVector4(Yellow));
	materialInstance->SetVec4("ubo.color", ToVector4(Red));
	materialInstance->SetTexture("image", nullptr);

	RenderFrame([&]() {
		_commandBuffer->SetLegacyMaterialInstance(materialInstance, 0);
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red)); // Red * Yellow

	RhiDevice::GetInstance()->WaitIdle();
	DefaultAllocator::GetInstance().Delete(materialInstance);
}

TEST_F(BindGroupRender, CreationFailsWhenTheResourcesDoNotMatchTheSet)
{
	Buffer* colors = CreateColorBlocks({White});

	// Set 1 has one uniform block and two texture blocks
	EXPECT_EQ(RhiDevice::GetInstance()->CreateBindGroup(_graphicsPipeline, 1, &colors, 1, nullptr, 0), nullptr);
	// The pipeline has no set 2
	EXPECT_EQ(RhiDevice::GetInstance()->CreateBindGroup(_graphicsPipeline, 2, &colors, 1, nullptr, 0), nullptr);

	_expectedErrorCount = 2;
}

#include <gtest/gtest.h>

#include "RenderTestHelpers.hpp"

#include "Shader/BindGroupTest_Fragment.hpp"
#include "Shader/BindGroupTest_Vertex.hpp"
#include "Shader/ConstantFirstTest_Vertex.hpp"
#include "Shader/DepthOnlyTest_Fragment.hpp"
#include "Shader/DepthOnlyTest_Vertex.hpp"
#include "Shader/SetFirstTest_Fragment.hpp"
#include "Shader/SetFirstTest_Vertex.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <HodEngine/Renderer/Renderer.hpp>

#include <HodEngine/RHI/BindGroup.hpp>
#include <HodEngine/RHI/Buffer.hpp>
#include <HodEngine/RHI/CommandBuffer.hpp>
#include <HodEngine/RHI/Fence.hpp>
#include <HodEngine/RHI/GraphicsPipeline.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Sampler.hpp>
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

		_renderTarget = rhiDevice->CreateRenderTarget();
		ASSERT_TRUE(_renderTarget->Init(DefaultTargetSize, DefaultTargetSize, true));

		_vertexShader = rhiDevice->CreateShader(Shader::ShaderType::Vertex);
		ASSERT_TRUE(_vertexShader->LoadFromIR(BindGroupTest_Vertex, BindGroupTest_Vertex_size, BindGroupTest_Vertex_reflection, BindGroupTest_Vertex_reflection_size));

		_fragmentShader = rhiDevice->CreateShader(Shader::ShaderType::Fragment);
		ASSERT_TRUE(
			_fragmentShader->LoadFromIR(BindGroupTest_Fragment, BindGroupTest_Fragment_size, BindGroupTest_Fragment_reflection, BindGroupTest_Fragment_reflection_size));

		_graphicsPipeline = CreatePipeline(GraphicsPipeline::CreateInfo());
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
		for (GraphicsPipeline* graphicsPipeline : _graphicsPipelines)
		{
			DefaultAllocator::GetInstance().Delete(graphicsPipeline);
		}
		DefaultAllocator::GetInstance().Delete(_vertexShader);
		DefaultAllocator::GetInstance().Delete(_fragmentShader);
		DefaultAllocator::GetInstance().Delete(_renderTarget);

		OutputCheckedTest::TearDown();
	}

	// A pipeline of the test shaders: they and their vertex inputs are set here, the rest is left to the caller
	GraphicsPipeline* CreatePipeline(GraphicsPipeline::CreateInfo createInfo)
	{
		StaticArray<VertexInput, 2> vertexInputs = {
			VertexInput(0, 0, VertexInput::Format::R32G32_SFloat),
			VertexInput(1, 8, VertexInput::Format::R32G32_SFloat),
		};

		createInfo._vertexInputs = vertexInputs.Data();
		createInfo._vertexInputCount = (uint32_t)vertexInputs.Size();
		createInfo._vertexShader = _vertexShader;
		createInfo._fragmentShader = _fragmentShader;

		GraphicsPipeline* graphicsPipeline = RhiDevice::GetInstance()->CreateGraphicsPipeline(createInfo);
		EXPECT_NE(graphicsPipeline, nullptr);
		if (graphicsPipeline != nullptr)
		{
			_graphicsPipelines.PushBack(graphicsPipeline);
		}
		return graphicsPipeline;
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

	// A null texture leaves the image to the fallback of the RHI, a null sampler means the default one of the texture
	BindGroup* CreateMaterialBindGroup(Buffer* colors, const Texture* texture, const Sampler* sampler = nullptr)
	{
		StaticArray<BindGroup::TextureBinding, 2> textureBindings;
		textureBindings[0]._texture = texture; // "image"
		textureBindings[1]._texture = texture; // "imageSampler"
		textureBindings[1]._sampler = sampler;

		BindGroup* bindGroup =
			RhiDevice::GetInstance()->CreateBindGroup(_graphicsPipeline, 1, &colors, 1, textureBindings.Data(), (uint32_t)textureBindings.Size());
		EXPECT_NE(bindGroup, nullptr);
		_bindGroups.PushBack(bindGroup);
		return bindGroup;
	}

	Texture* CreateWhiteTexture()
	{
		const uint8_t pixels[4 * 2 * 2] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		EXPECT_TRUE(texture->Build(TextureCreateInfo(2, 2), pixels));
		_textures.PushBack(texture);
		return texture;
	}

	// 2x2 texture, row by row from the first texel: Red, Green / Blue, Yellow
	Texture* CreateFourColorsTexture()
	{
		const uint8_t pixels[4 * 2 * 2] = {255, 0, 0, 255, 0, 255, 0, 255, 0, 0, 255, 255, 255, 255, 0, 255};

		Sampler::CreateInfo nearest;
		nearest._filterMode = FilterMode::Nearest;

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		EXPECT_TRUE(texture->Build(TextureCreateInfo(2, 2), pixels));
		texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(nearest));
		_textures.PushBack(texture);
		return texture;
	}

	// size x size texture with all its mip levels, each of one color: levelColors[level], or the last one past them. Read with a nearest filter
	Texture* CreateMipColorsTexture(uint32_t size, std::initializer_list<Color> levelColors)
	{
		const uint32_t mipLevelCount = Texture::ComputeMipLevelCount(size, size);

		Vector<uint8_t> pixels;
		pixels.Resize(Texture::ComputeMipChainByteSize(size, size, mipLevelCount));
		uint8_t* texel = pixels.Data();
		for (uint32_t mipLevel = 0; mipLevel < mipLevelCount; ++mipLevel)
		{
			const Color&   color = *(levelColors.begin() + Min(mipLevel, (uint32_t)levelColors.size() - 1));
			const uint32_t mipSize = Texture::ComputeMipSize(size, mipLevel);
			for (uint32_t texelIndex = 0; texelIndex < mipSize * mipSize; ++texelIndex)
			{
				*texel++ = (uint8_t)(color.r * 255.0f);
				*texel++ = (uint8_t)(color.g * 255.0f);
				*texel++ = (uint8_t)(color.b * 255.0f);
				*texel++ = (uint8_t)(color.a * 255.0f);
			}
		}

		Texture::CreateInfo createInfo = TextureCreateInfo(size, size);
		createInfo._mipLevelCount = mipLevelCount;

		Sampler::CreateInfo nearest;
		nearest._filterMode = FilterMode::Nearest;

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		EXPECT_TRUE(texture->Build(createInfo, pixels.Data()));
		texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(nearest));
		_textures.PushBack(texture);
		return texture;
	}

	// Records one render pass on the target, submits it and blocks until it can be read back
	void RenderFrame(const std::function<void()>& draw)
	{
		RhiDevice* rhiDevice = RhiDevice::GetInstance();

		Renderer::GetInstance()->AcquireNextFrame();

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
		if (_frameTexture != nullptr) // a target without color has nothing to read back
		{
			_frameTexture->CaptureReadback(_commandBuffer);
		}
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
	GraphicsPipeline* _graphicsPipeline = nullptr; // the one RenderFrame draws with
	uint32_t          _blockStride = 0;

	CommandBuffer* _commandBuffer = nullptr;
	Texture*       _frameTexture = nullptr;

	Vector<GraphicsPipeline*> _graphicsPipelines;
	Vector<BindGroup*>        _bindGroups;
	Vector<Buffer*>           _buffers;
	Vector<Texture*>          _textures;
};

// A RenderTarget is the same texture frame after frame: what a frame draws replaces what the previous one drew
TEST_F(BindGroupRender, RenderTargetKeepsItsTextureFromFrameToFrame)
{
	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red, Green}), CreateWhiteTexture());

	Texture* colorTexture = _renderTarget->GetColorTexture();

	for (uint32_t frame = 0; frame < 4; ++frame)
	{
		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, frame % 2);
			DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
		});

		EXPECT_EQ(_renderTarget->GetColorTexture(), colorTexture);
		EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), frame % 2 == 0 ? Red : Green)) << "frame " << frame;
	}
}

// The same BindGroup serves several draws, each reading its own block of the uniform buffer
TEST_F(BindGroupRender, UniformBufferOffsetSelectsTheBlockOfEachDraw)
{
	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red, Green, Blue}), CreateWhiteTexture());

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
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Yellow, White}), CreateWhiteTexture());

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

// Two BindGroups of the same set, differing by their texture, used in the same frame.
// The one given no texture gets the fallback of the RHI.
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
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), FallbackColor));
}

// A BindGroup is immutable, what a later frame sees is whatever its buffers hold then
TEST_F(BindGroupRender, BufferContentCanChangeBetweenFrames)
{
	Buffer* colors = CreateColorBlocks({Red});

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(colors, CreateWhiteTexture());

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

// The same texture read through two samplers: the texels of the four colors texture, or a blend of them
TEST_F(BindGroupRender, SamplerIsGivenApartFromTheTexture)
{
	Sampler::CreateInfo linear;
	linear._filterMode = FilterMode::Linear;

	Buffer*  colors = CreateColorBlocks({White});
	Texture* texture = CreateFourColorsTexture(); // its default sampler is a nearest one

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* withItsSampler = CreateMaterialBindGroup(colors, texture);
	BindGroup* withLinearSampler = CreateMaterialBindGroup(colors, texture, RhiDevice::GetInstance()->GetSampler(linear));

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);

		SetBindGroup(1, withItsSampler, 0);
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f);

		SetBindGroup(1, withLinearSampler, 0);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f);
	});

	// Just past the center of each quad, towards its (right, top) texel
	EXPECT_TRUE(ColorNear(ReadPixel(-0.47f, 0.06f), Yellow));
	EXPECT_TRUE(IsBlended(ReadPixel(0.53f, 0.06f)));
}

// The default sampler of a texture is not part of the texture: it can change without rebuilding it
TEST_F(BindGroupRender, DefaultSamplerOfATextureCanChange)
{
	Sampler::CreateInfo linear;
	linear._filterMode = FilterMode::Linear;

	Buffer*  colors = CreateColorBlocks({White});
	Texture* texture = CreateFourColorsTexture();

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));

	auto draw = [&](BindGroup* material) {
		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, 0);
			DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
		});
	};

	draw(CreateMaterialBindGroup(colors, texture));
	EXPECT_TRUE(ColorNear(ReadPixel(0.06f, 0.06f), Yellow));

	texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(linear));

	// A BindGroup is immutable: the new default applies to the ones created from now on
	draw(CreateMaterialBindGroup(colors, texture));
	EXPECT_TRUE(IsBlended(ReadPixel(0.06f, 0.06f)));
}

// Down to 1 x 1, a side halved at each level, rounded down, 1 at least
TEST(TextureMipChain, LevelsAndSizes)
{
	EXPECT_EQ(Texture::ComputeMipLevelCount(64, 64), 7u);
	EXPECT_EQ(Texture::ComputeMipLevelCount(100, 30), 7u);
	EXPECT_EQ(Texture::ComputeMipLevelCount(1, 1), 1u);
	EXPECT_EQ(Texture::ComputeMipLevelCount(0, 0), 0u);

	EXPECT_EQ(Texture::ComputeMipSize(100, 3), 12u);
	EXPECT_EQ(Texture::ComputeMipSize(30, 5), 1u);

	EXPECT_EQ(Texture::ComputeMipChainByteSize(4, 2, 3), (4u * 2u + 2u * 1u + 1u * 1u) * 4u);
}

// The sampler reads the mip level of the size the texture is drawn at: 64 texels over 64, 32 and 8 pixels
TEST_F(BindGroupRender, MipLevelIsChosenByTheSizeTheTextureIsDrawnAt)
{
	Texture* texture = CreateMipColorsTexture(64, {Red, Green, Blue, Yellow, White});
	EXPECT_EQ(texture->GetMipLevelCount(), 7u);

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({White}), texture);

	auto drawCentered = [&](float halfSize) {
		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, 0);
			DrawQuad(-halfSize, -halfSize, halfSize, halfSize);
		});
	};

	drawCentered(1.0f);
	EXPECT_TRUE(ColorNear(ReadPixel(0.03f, 0.03f), Red));

	drawCentered(0.5f);
	EXPECT_TRUE(ColorNear(ReadPixel(0.03f, 0.03f), Green));

	drawCentered(0.125f);
	EXPECT_TRUE(ColorNear(ReadPixel(0.03f, 0.03f), Yellow));
}

// A texture built again is a new one for what is created from it: its generation changes, the BindGroups made after see its new levels
TEST_F(BindGroupRender, TextureBuiltAgainGetsItsNewMipLevels)
{
	Texture* texture = CreateMipColorsTexture(64, {Red});
	const uint32_t firstGeneration = texture->GetGeneration();

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	Buffer*    colors = CreateColorBlocks({White});

	auto drawSmall = [&](BindGroup* material) {
		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, 0);
			DrawQuad(-0.125f, -0.125f, 0.125f, 0.125f);
		});
	};

	drawSmall(CreateMaterialBindGroup(colors, texture));
	EXPECT_TRUE(ColorNear(ReadPixel(0.03f, 0.03f), Red));

	// The level 0 alone: what is drawn small reads it too
	const uint8_t       greenPixels[4 * 2 * 2] = {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255};
	ASSERT_TRUE(texture->Build(TextureCreateInfo(2, 2), greenPixels));
	EXPECT_NE(texture->GetGeneration(), firstGeneration);
	EXPECT_EQ(texture->GetMipLevelCount(), 1u);
	EXPECT_EQ(texture->GetWidth(), 2u);

	drawSmall(CreateMaterialBindGroup(colors, texture));
	EXPECT_TRUE(ColorNear(ReadPixel(0.03f, 0.03f), Green));
}

// A texture with no default sampler is read with the default settings: a linear filter
TEST_F(BindGroupRender, TextureWithoutDefaultSamplerIsReadLinearly)
{
	Texture* texture = CreateFourColorsTexture();
	texture->SetDefaultSampler(nullptr);

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({White}), texture);

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);
		SetBindGroup(1, material, 0);
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	});

	EXPECT_TRUE(IsBlended(ReadPixel(0.06f, 0.06f)));
}

TEST_F(BindGroupRender, SamplersAreSharedByTheirSettings)
{
	Sampler::CreateInfo nearest;
	nearest._filterMode = FilterMode::Nearest;

	Sampler::CreateInfo nearestRepeat = nearest;
	nearestRepeat._wrapMode = WrapMode::Repeat;

	const Sampler* sampler = RhiDevice::GetInstance()->GetSampler(nearest);
	ASSERT_NE(sampler, nullptr);

	EXPECT_EQ(RhiDevice::GetInstance()->GetSampler(nearest), sampler);
	EXPECT_NE(RhiDevice::GetInstance()->GetSampler(nearestRepeat), sampler);

	EXPECT_EQ(CreateFourColorsTexture()->GetDefaultSampler(), sampler);
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

// ============================================================================
// Lifetime and capacity
// ============================================================================

// Far more BindGroups than one descriptor pool of the Vulkan backend holds: the device takes another one when a pool is full
TEST_F(BindGroupRender, ManyBindGroupsCoexist)
{
	constexpr uint32_t BindGroupCount = 3000;

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	Buffer*    colors = CreateColorBlocks({Red, Green});
	Texture*   texture = CreateWhiteTexture();

	BindGroup* first = nullptr;
	BindGroup* last = nullptr;
	for (uint32_t index = 0; index < BindGroupCount; ++index)
	{
		BindGroup* bindGroup = CreateMaterialBindGroup(colors, texture);
		ASSERT_NE(bindGroup, nullptr) << "BindGroup " << index;
		if (first == nullptr)
		{
			first = bindGroup;
		}
		last = bindGroup;
	}

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);

		SetBindGroup(1, first, 0);
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f);

		SetBindGroup(1, last, 1);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Green));
}

// A buffer may be deleted while the GPU still draws with it: it is only released once the frames in flight are done with it.
// The graphics API validation reports a buffer destroyed while in use.
TEST_F(BindGroupRender, BufferDeletedWhileInFlightIsKeptAlive)
{
	RhiDevice* rhiDevice = RhiDevice::GetInstance();

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red}), CreateWhiteTexture());

	// Not owned by the fixture: deleted by the test
	auto createBuffer = [&](Buffer::Usage usage, const void* data, uint32_t size)
	{
		Buffer* buffer = rhiDevice->CreateBuffer(usage, size);
		void*   bufferData = buffer->Lock();
		EXPECT_NE(bufferData, nullptr);
		if (bufferData != nullptr)
		{
			std::memcpy(bufferData, data, size);
			buffer->Unlock();
		}
		return buffer;
	};

	StaticArray<Vector2, 4>  positions = {Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f)};
	StaticArray<Vector2, 4>  uvs = {Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f)};
	StaticArray<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

	StaticArray<Buffer*, 2> vertexBuffers = {
		createBuffer(Buffer::Usage::Vertex, positions.Data(), (uint32_t)(positions.Size() * sizeof(Vector2))),
		createBuffer(Buffer::Usage::Vertex, uvs.Data(), (uint32_t)(uvs.Size() * sizeof(Vector2))),
	};
	Buffer* indexBuffer = createBuffer(Buffer::Usage::Index, indices.Data(), (uint32_t)(indices.Size() * sizeof(uint16_t)));

	Renderer::GetInstance()->AcquireNextFrame();

	_frameTexture = _renderTarget->GetColorTexture();

	Rect viewport;
	viewport._position = Vector2(0.0f, 0.0f);
	viewport._size = Vector2((float)DefaultTargetSize, (float)DefaultTargetSize);

	_commandBuffer = rhiDevice->CreateCommandBuffer();
	ASSERT_TRUE(_commandBuffer->StartRecord());
	_commandBuffer->StartRenderPass(_renderTarget);
	_commandBuffer->SetViewport(viewport);
	_commandBuffer->SetGraphicsPipeline(_graphicsPipeline);
	Matrix4 mvp = Matrix4::Identity;
	_commandBuffer->SetConstant(&mvp, sizeof(mvp), Shader::ShaderType::Vertex);
	SetBindGroup(0, global, 0);
	SetBindGroup(1, material, 0);
	_commandBuffer->SetVertexBuffer(vertexBuffers.Data(), (uint32_t)vertexBuffers.Size());
	_commandBuffer->SetIndexBuffer(indexBuffer);
	_commandBuffer->DrawIndexed((uint32_t)indices.Size(), 0, 0);
	_commandBuffer->EndRenderPass();
	_frameTexture->CaptureReadback(_commandBuffer);
	_commandBuffer->EndRecord();

	Fence* fence = rhiDevice->CreateFence();
	fence->Reset();
	rhiDevice->SubmitCommandBuffers(&_commandBuffer, 1, nullptr, nullptr, fence);

	// Before the GPU is done with them
	DefaultAllocator::GetInstance().Delete(vertexBuffers[0]);
	DefaultAllocator::GetInstance().Delete(vertexBuffers[1]);
	DefaultAllocator::GetInstance().Delete(indexBuffer);

	fence->Wait();
	DefaultAllocator::GetInstance().Delete(fence);
	DefaultAllocator::GetInstance().Delete(_commandBuffer);
	_commandBuffer = nullptr;

	Renderer::GetInstance()->Render();

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
}

// Texture uploads are recorded without the CPU waiting for them, and submitted ahead of the next frame: many in a row,
// each ready for the frame that draws with it
TEST_F(BindGroupRender, ManyTexturesUploadedWithoutWaiting)
{
	constexpr uint32_t TextureCount = 300;

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	Buffer*    colors = CreateColorBlocks({White});

	auto createTexture = [&](uint8_t red, uint8_t green)
	{
		const uint8_t pixels[4 * 2 * 2] = {red, green, 0, 255, red, green, 0, 255, red, green, 0, 255, red, green, 0, 255};

		Texture* texture = RhiDevice::GetInstance()->CreateTexture();
		EXPECT_TRUE(texture->Build(TextureCreateInfo(2, 2), pixels));
		_textures.PushBack(texture);
		return texture;
	};

	BindGroup* first = CreateMaterialBindGroup(colors, createTexture(255, 0));
	for (uint32_t index = 1; index < TextureCount - 1; ++index)
	{
		createTexture((uint8_t)index, 128);
	}
	BindGroup* last = CreateMaterialBindGroup(colors, createTexture(0, 255));

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);

		SetBindGroup(1, first, 0);
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f);

		SetBindGroup(1, last, 0);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Green));
}

// The upload of a texture deleted right after being built is still submitted with the next frame: the texture is kept until it is done.
// The graphics API validation reports a resource used after being destroyed.
TEST_F(BindGroupRender, TextureDeletedBeforeItsUploadIsSubmitted)
{
	const uint8_t pixels[4 * 2 * 2] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

	Texture* texture = RhiDevice::GetInstance()->CreateTexture();
	ASSERT_TRUE(texture->Build(TextureCreateInfo(2, 2), pixels));
	DefaultAllocator::GetInstance().Delete(texture);

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red}), CreateWhiteTexture());

	for (uint32_t frame = 0; frame < 3; ++frame) // past the frames in flight: what is deferred is released meanwhile
	{
		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, 0);
			DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
		});
	}

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
}

// Several frames submitted without waiting for any, each releasing what it drew with right after submitting it: what is released
// is kept until the GPU is done with the submissions that may use it, whatever the frame.
// The graphics API validation reports what is destroyed while still in use.
TEST_F(BindGroupRender, ResourcesReleasedEveryFrameWithoutWaitingForTheGpu)
{
	constexpr uint32_t FrameCount = 6;

	RhiDevice* rhiDevice = RhiDevice::GetInstance();

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));

	auto createBuffer = [&](Buffer::Usage usage, const void* data, uint32_t size)
	{
		Buffer* buffer = rhiDevice->CreateBuffer(usage, size);
		void*   bufferData = buffer->Lock();
		EXPECT_NE(bufferData, nullptr);
		if (bufferData != nullptr)
		{
			std::memcpy(bufferData, data, size);
			buffer->Unlock();
		}
		return buffer;
	};

	StaticArray<Vector2, 4>  positions = {Vector2(-1.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(-1.0f, -1.0f), Vector2(1.0f, -1.0f)};
	StaticArray<Vector2, 4>  uvs = {Vector2(0.0f, 1.0f), Vector2(1.0f, 1.0f), Vector2(0.0f, 0.0f), Vector2(1.0f, 0.0f)};
	StaticArray<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};
	const uint8_t            pixels[4 * 2 * 2] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};
	Vector4                  red = ToVector4(Red);

	Rect viewport;
	viewport._position = Vector2(0.0f, 0.0f);
	viewport._size = Vector2((float)DefaultTargetSize, (float)DefaultTargetSize);

	Vector<CommandBuffer*> commandBuffers; // deleted once the GPU is done with them, as the RHI asks

	for (uint32_t frame = 0; frame < FrameCount; ++frame)
	{
		Renderer::GetInstance()->AcquireNextFrame();

		Texture* texture = rhiDevice->CreateTexture();
		ASSERT_TRUE(texture->Build(TextureCreateInfo(2, 2), pixels));
		Buffer* colors = createBuffer(Buffer::Usage::Uniform, &red, sizeof(red));

		StaticArray<BindGroup::TextureBinding, 2> textureBindings;
		textureBindings[0]._texture = texture;
		textureBindings[1]._texture = texture;
		BindGroup* material = rhiDevice->CreateBindGroup(_graphicsPipeline, 1, &colors, 1, textureBindings.Data(), (uint32_t)textureBindings.Size());
		ASSERT_NE(material, nullptr);

		StaticArray<Buffer*, 2> vertexBuffers = {
			createBuffer(Buffer::Usage::Vertex, positions.Data(), (uint32_t)(positions.Size() * sizeof(Vector2))),
			createBuffer(Buffer::Usage::Vertex, uvs.Data(), (uint32_t)(uvs.Size() * sizeof(Vector2))),
		};
		Buffer* indexBuffer = createBuffer(Buffer::Usage::Index, indices.Data(), (uint32_t)(indices.Size() * sizeof(uint16_t)));

		_commandBuffer = rhiDevice->CreateCommandBuffer();
		ASSERT_TRUE(_commandBuffer->StartRecord());
		_commandBuffer->StartRenderPass(_renderTarget);
		_commandBuffer->SetViewport(viewport);
		_commandBuffer->SetGraphicsPipeline(_graphicsPipeline);
		Matrix4 mvp = Matrix4::Identity;
		_commandBuffer->SetConstant(&mvp, sizeof(mvp), Shader::ShaderType::Vertex);
		SetBindGroup(0, global, 0);
		SetBindGroup(1, material, 0);
		_commandBuffer->SetVertexBuffer(vertexBuffers.Data(), (uint32_t)vertexBuffers.Size());
		_commandBuffer->SetIndexBuffer(indexBuffer);
		_commandBuffer->DrawIndexed((uint32_t)indices.Size(), 0, 0);
		_commandBuffer->EndRenderPass();
		_renderTarget->GetColorTexture()->CaptureReadback(_commandBuffer);
		_commandBuffer->EndRecord();

		rhiDevice->SubmitCommandBuffers(&_commandBuffer, 1, nullptr, nullptr, nullptr);
		commandBuffers.PushBack(_commandBuffer);
		_commandBuffer = nullptr;

		// Right after the submission, the GPU not being waited for
		DefaultAllocator::GetInstance().Delete(material);
		DefaultAllocator::GetInstance().Delete(texture);
		DefaultAllocator::GetInstance().Delete(colors);
		DefaultAllocator::GetInstance().Delete(vertexBuffers[0]);
		DefaultAllocator::GetInstance().Delete(vertexBuffers[1]);
		DefaultAllocator::GetInstance().Delete(indexBuffer);

		Renderer::GetInstance()->Render();
	}

	rhiDevice->WaitIdle();
	for (CommandBuffer* commandBuffer : commandBuffers)
	{
		DefaultAllocator::GetInstance().Delete(commandBuffer);
	}

	_frameTexture = _renderTarget->GetColorTexture();
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
}

// ============================================================================
// What GraphicsPipeline::CreateInfo sets apart from the shaders
// ============================================================================

class PipelineStateRender : public BindGroupRender
{
protected:
	// Clockwise quad on the left half of the target, counter-clockwise one on the right half
	void RenderBothWindings(CullMode cullMode)
	{
		GraphicsPipeline::CreateInfo createInfo;
		createInfo._cullMode = cullMode;
		_graphicsPipeline = CreatePipeline(createInfo);

		BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
		BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red}), CreateWhiteTexture());

		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, 0);
			DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f);
			DrawQuad(1.0f, -1.0f, 0.0f, 1.0f); // mirrored, which reverses its winding
		});
	}

	// What a half transparent quad gives once drawn over the Background
	Color RenderOverBackground(BlendMode blendMode)
	{
		GraphicsPipeline::CreateInfo createInfo;
		createInfo._blendMode = blendMode;
		_graphicsPipeline = CreatePipeline(createInfo);

		BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
		BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Color(0.8f, 0.4f, 0.0f, 0.5f)}), CreateWhiteTexture());

		RenderFrame([&]() {
			SetBindGroup(0, global, 0);
			SetBindGroup(1, material, 0);
			DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
		});

		return ReadPixel(0.0f, 0.0f);
	}
};

TEST_F(PipelineStateRender, CullModeNoneDrawsBothFaces)
{
	RenderBothWindings(CullMode::None);

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Red));
}

// The front face is the counter-clockwise one, on every backend
TEST_F(PipelineStateRender, CullModeBackDrawsCounterClockwiseTriangles)
{
	RenderBothWindings(CullMode::Back);

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Red));
}

TEST_F(PipelineStateRender, CullModeFrontDrawsClockwiseTriangles)
{
	RenderBothWindings(CullMode::Front);

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Background));
}

TEST_F(PipelineStateRender, BlendModeOpaqueReplacesTheTarget)
{
	EXPECT_TRUE(ColorNear(RenderOverBackground(BlendMode::Opaque), Color(0.8f, 0.4f, 0.0f, 0.5f)));
}

TEST_F(PipelineStateRender, BlendModeAlpha)
{
	// The alpha is blended like the color: 0.5 * 0.5 + 1.0 * 0.5
	EXPECT_TRUE(ColorNear(RenderOverBackground(BlendMode::Alpha), Color(0.45f, 0.25f, 0.05f, 0.75f)));
}

TEST_F(PipelineStateRender, BlendModePremultipliedAlpha)
{
	EXPECT_TRUE(ColorNear(RenderOverBackground(BlendMode::PremultipliedAlpha), Color(0.85f, 0.45f, 0.05f, 1.0f)));
}

TEST_F(PipelineStateRender, BlendModeAdditive)
{
	EXPECT_TRUE(ColorNear(RenderOverBackground(BlendMode::Additive), Color(0.5f, 0.3f, 0.1f, 1.0f)));
}

// ============================================================================
// Depth: z goes from 0, the nearest, to 1, the farthest
// ============================================================================

class DepthRender : public BindGroupRender
{
protected:
	void SetUp() override
	{
		BindGroupRender::SetUp();

		_global = CreateGlobalBindGroup(CreateColorBlocks({White}));
		_material = CreateMaterialBindGroup(CreateColorBlocks({Red, Green, Blue}), CreateWhiteTexture());
	}

	GraphicsPipeline* CreateDepthPipeline(bool depthTest, bool depthWrite)
	{
		GraphicsPipeline::CreateInfo createInfo;
		createInfo._depthTest = depthTest;
		createInfo._depthWrite = depthWrite;
		return CreatePipeline(createInfo);
	}

	// Quad covering the target, of the color 'colorIndex' of _material (Red, Green, Blue)
	void DrawAtDepth(const GraphicsPipeline* graphicsPipeline, float depth, uint32_t colorIndex)
	{
		_commandBuffer->SetGraphicsPipeline(graphicsPipeline);

		// A new pipeline takes its constant and its BindGroups to be set again
		Matrix4 mvp = Matrix4::Translation(Vector4(0.0f, 0.0f, depth, 1.0f)); // positions are given in clip space, only their depth is set
		_commandBuffer->SetConstant(&mvp, sizeof(mvp), Shader::ShaderType::Vertex);
		SetBindGroup(0, _global, 0);
		SetBindGroup(1, _material, colorIndex);

		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	}

protected:
	BindGroup* _global = nullptr;
	BindGroup* _material = nullptr;
};

TEST_F(DepthRender, WithoutDepthTestTheLastDrawIsOnTop)
{
	GraphicsPipeline* noDepth = CreateDepthPipeline(false, false);

	RenderFrame([&]() {
		DrawAtDepth(noDepth, 0.2f, 0);
		DrawAtDepth(noDepth, 0.8f, 1);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

TEST_F(DepthRender, DepthTestKeepsTheNearestWhateverTheOrder)
{
	GraphicsPipeline* depth = CreateDepthPipeline(true, true);

	RenderFrame([&]() {
		DrawAtDepth(depth, 0.2f, 0);
		DrawAtDepth(depth, 0.8f, 1);
	});
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));

	RenderFrame([&]() {
		DrawAtDepth(depth, 0.8f, 1);
		DrawAtDepth(depth, 0.2f, 0);
	});
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
}

// What a frame leaves in the depth buffer does not hide what the next one draws
TEST_F(DepthRender, DepthIsClearedByEachRenderPass)
{
	GraphicsPipeline* depth = CreateDepthPipeline(true, true);

	RenderFrame([&]() { DrawAtDepth(depth, 0.2f, 0); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));

	RenderFrame([&]() { DrawAtDepth(depth, 0.8f, 1); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

TEST_F(DepthRender, WithoutDepthWriteTheDepthIsLeftAsItIs)
{
	GraphicsPipeline* depth = CreateDepthPipeline(true, true);
	GraphicsPipeline* testOnly = CreateDepthPipeline(true, false);

	RenderFrame([&]() {
		DrawAtDepth(testOnly, 0.2f, 0);
		DrawAtDepth(depth, 0.8f, 1); // farther, but nothing was written at 0.2
	});

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

TEST_F(DepthRender, DepthWriteWithoutDepthTestAlwaysDraws)
{
	GraphicsPipeline* depth = CreateDepthPipeline(true, true);
	GraphicsPipeline* writeOnly = CreateDepthPipeline(false, true);

	RenderFrame([&]() {
		DrawAtDepth(depth, 0.2f, 0);
		DrawAtDepth(writeOnly, 0.6f, 1); // drawn although farther, and the depth becomes 0.6
	});
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));

	RenderFrame([&]() {
		DrawAtDepth(depth, 0.2f, 0);
		DrawAtDepth(writeOnly, 0.6f, 1);
		DrawAtDepth(depth, 0.4f, 2); // nearer than 0.6, not than the 0.2 it replaced
	});
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Blue));
}

// The depth buffer is 16 bits: layers drawn by the editor camera, which sees from z = -1024 to 1024, still sort a tenth of a unit apart
TEST_F(DepthRender, CloseLayersSortThroughTheEditorDepthRange)
{
	GraphicsPipeline* depth = CreateDepthPipeline(true, true);

	Matrix4 projection = Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1024.0f, 1024.0f);
	auto    drawLayer = [&](float z, uint32_t colorIndex)
	{
		_commandBuffer->SetGraphicsPipeline(depth);
		Matrix4 mvp = projection * Matrix4::Translation(Vector4(0.0f, 0.0f, z, 1.0f));
		_commandBuffer->SetConstant(&mvp, sizeof(mvp), Shader::ShaderType::Vertex);
		SetBindGroup(0, _global, 0);
		SetBindGroup(1, _material, colorIndex);
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	};

	// The greater the z, the nearer
	RenderFrame([&]() {
		drawLayer(0.1f, 0);
		drawLayer(0.0f, 1);
	});
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));

	RenderFrame([&]() {
		drawLayer(0.0f, 1);
		drawLayer(0.1f, 0);
	});
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
}

// ============================================================================
// Render targets without a color or a depth texture, and the pipelines created for them
// ============================================================================

class TargetFormatRender : public BindGroupRender
{
protected:
	// Replaces the render target RenderFrame draws into
	void UseRenderTarget(PixelFormat colorFormat, PixelFormat depthFormat)
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
		ASSERT_TRUE(_renderTarget->Init(DefaultTargetSize, DefaultTargetSize, true, colorFormat, depthFormat));
	}

	GraphicsPipeline* CreatePipelineFor(PixelFormat colorFormat, PixelFormat depthFormat, bool depthWrite = false)
	{
		GraphicsPipeline::CreateInfo createInfo;
		createInfo._colorFormat = colorFormat;
		createInfo._depthFormat = depthFormat;
		createInfo._depthTest = depthWrite;
		createInfo._depthWrite = depthWrite;
		return CreatePipeline(createInfo);
	}
};

// What a post-process pass draws into
TEST_F(TargetFormatRender, ColorOnlyTarget)
{
	UseRenderTarget(PixelFormat::R8G8B8A8_UNorm, PixelFormat::None);
	_graphicsPipeline = CreatePipelineFor(PixelFormat::R8G8B8A8_UNorm, PixelFormat::None);
	ASSERT_NE(_graphicsPipeline, nullptr);

	BindGroup* global = CreateGlobalBindGroup(CreateColorBlocks({White}));
	BindGroup* material = CreateMaterialBindGroup(CreateColorBlocks({Red}), CreateWhiteTexture());

	RenderFrame([&]() {
		SetBindGroup(0, global, 0);
		SetBindGroup(1, material, 0);
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f);
	});

	EXPECT_EQ(_renderTarget->GetDepthTexture(), nullptr);
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
}

// What a shadow map is drawn into, with shaders that write no color. Nothing to read back: the graphics API validation is what checks it.
TEST_F(TargetFormatRender, DepthOnlyTarget)
{
	UseRenderTarget(PixelFormat::None, PixelFormat::D16_UNorm);

	RhiDevice* rhiDevice = RhiDevice::GetInstance();

	Shader* vertexShader = rhiDevice->CreateShader(Shader::ShaderType::Vertex);
	ASSERT_TRUE(vertexShader->LoadFromIR(DepthOnlyTest_Vertex, DepthOnlyTest_Vertex_size, DepthOnlyTest_Vertex_reflection, DepthOnlyTest_Vertex_reflection_size));
	Shader* fragmentShader = rhiDevice->CreateShader(Shader::ShaderType::Fragment);
	ASSERT_TRUE(
		fragmentShader->LoadFromIR(DepthOnlyTest_Fragment, DepthOnlyTest_Fragment_size, DepthOnlyTest_Fragment_reflection, DepthOnlyTest_Fragment_reflection_size));

	VertexInput vertexInput(0, 0, VertexInput::Format::R32G32_SFloat);

	GraphicsPipeline::CreateInfo createInfo;
	createInfo._vertexInputs = &vertexInput;
	createInfo._vertexInputCount = 1;
	createInfo._vertexShader = vertexShader;
	createInfo._fragmentShader = fragmentShader;
	createInfo._colorFormat = PixelFormat::None;
	createInfo._depthTest = true;
	createInfo._depthWrite = true;
	_graphicsPipeline = rhiDevice->CreateGraphicsPipeline(createInfo);
	ASSERT_NE(_graphicsPipeline, nullptr);
	_graphicsPipelines.PushBack(_graphicsPipeline);

	// The shaders are no longer needed once the pipeline is created
	DefaultAllocator::GetInstance().Delete(vertexShader);
	DefaultAllocator::GetInstance().Delete(fragmentShader);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f); });

	EXPECT_EQ(_renderTarget->GetColorTexture(), nullptr);
}

TEST_F(TargetFormatRender, PipelineOfOtherFormatsIsNotBound)
{
	GraphicsPipeline* colorOnly = CreatePipelineFor(PixelFormat::R8G8B8A8_UNorm, PixelFormat::None);
	ASSERT_NE(colorOnly, nullptr);

	// The target has a depth texture, the pipeline is not created for one
	RenderFrame([&]() { _commandBuffer->SetGraphicsPipeline(colorOnly); });

	_expectedErrorCount = 1;
}

TEST_F(TargetFormatRender, DepthStatesNeedADepthFormat)
{
	StaticArray<VertexInput, 2> vertexInputs = {
		VertexInput(0, 0, VertexInput::Format::R32G32_SFloat),
		VertexInput(1, 8, VertexInput::Format::R32G32_SFloat),
	};

	GraphicsPipeline::CreateInfo createInfo;
	createInfo._vertexInputs = vertexInputs.Data();
	createInfo._vertexInputCount = (uint32_t)vertexInputs.Size();
	createInfo._vertexShader = _vertexShader;
	createInfo._fragmentShader = _fragmentShader;
	createInfo._depthTest = true;
	createInfo._depthFormat = PixelFormat::None;

	EXPECT_EQ(RhiDevice::GetInstance()->CreateGraphicsPipeline(createInfo), nullptr);

	_expectedErrorCount = 1;
}

TEST_F(TargetFormatRender, RenderTargetNeedsATexture)
{
	RenderTarget* renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
	EXPECT_FALSE(renderTarget->Init(DefaultTargetSize, DefaultTargetSize, false, PixelFormat::None, PixelFormat::None));
	DefaultAllocator::GetInstance().Delete(renderTarget);

	_expectedErrorCount = 1;
}

// ============================================================================
// Where each stage reads the push constant and the sets from.
//
// Shader/ConstantFirstTest.slang and Shader/SetFirstTest.slang are the same shader but for the order they declare their push constant
// and their set in. On Metal, where the compiler numbers the buffers in that order, the constant and the set are not at the same buffer
// index in each of them, and a pipeline can take its stages from both: each stage reads them from where its own shader was compiled to.
// ============================================================================

class StageBindingRender : public BindGroupRender
{
protected:
	void TearDown() override
	{
		BindGroupRender::TearDown();

		for (Shader* shader : _stageShaders)
		{
			DefaultAllocator::GetInstance().Delete(shader);
		}
	}

	Shader* LoadShader(Shader::ShaderType shaderType, const void* bytecode, uint32_t bytecodeSize, const char* reflection, uint32_t reflectionSize)
	{
		Shader* shader = RhiDevice::GetInstance()->CreateShader(shaderType);
		EXPECT_TRUE(shader->LoadFromIR(bytecode, bytecodeSize, reflection, reflectionSize));
		_stageShaders.PushBack(shader);
		return shader;
	}

	// Becomes the one RenderFrame draws with
	void UseStages(Shader* vertexShader, Shader* fragmentShader)
	{
		StaticArray<VertexInput, 2> vertexInputs = {
			VertexInput(0, 0, VertexInput::Format::R32G32_SFloat),
			VertexInput(1, 8, VertexInput::Format::R32G32_SFloat),
		};

		GraphicsPipeline::CreateInfo createInfo;
		createInfo._vertexInputs = vertexInputs.Data();
		createInfo._vertexInputCount = (uint32_t)vertexInputs.Size();
		createInfo._vertexShader = vertexShader;
		createInfo._fragmentShader = fragmentShader;

		_graphicsPipeline = RhiDevice::GetInstance()->CreateGraphicsPipeline(createInfo);
		ASSERT_NE(_graphicsPipeline, nullptr);
		_graphicsPipelines.PushBack(_graphicsPipeline);
	}

	// The left half of the target moved to the right half by the constant, of the color of the set read by both stages:
	// drawn of that color on the right only if each stage read the constant and the set from where its shader has them
	void DrawMovedQuad(const Color& color)
	{
		Buffer*    colors = CreateColorBlocks({color});
		BindGroup* bindGroup = RhiDevice::GetInstance()->CreateBindGroup(_graphicsPipeline, 0, &colors, 1, nullptr, 0);
		ASSERT_NE(bindGroup, nullptr);
		_bindGroups.PushBack(bindGroup);

		RenderFrame([&]() {
			Matrix4 mvp = Matrix4::Translation(Vector2(1.0f, 0.0f));
			_commandBuffer->SetConstant(&mvp, sizeof(mvp), Shader::ShaderType::Vertex);
			SetBindGroup(0, bindGroup, 0);
			DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f);
		});
	}

protected:
	Vector<Shader*> _stageShaders;
};

// The constant is not always at the first buffer: setting it does not overwrite the set there
TEST_F(StageBindingRender, ConstantDeclaredAfterASet)
{
	UseStages(LoadShader(Shader::ShaderType::Vertex, SetFirstTest_Vertex, SetFirstTest_Vertex_size, SetFirstTest_Vertex_reflection, SetFirstTest_Vertex_reflection_size),
	          LoadShader(Shader::ShaderType::Fragment, SetFirstTest_Fragment, SetFirstTest_Fragment_size, SetFirstTest_Fragment_reflection,
	                     SetFirstTest_Fragment_reflection_size));

	DrawMovedQuad(Red);

	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Background));
}

// The vertex stage has the constant at the first buffer and the set at the second, the fragment stage the set at the first
TEST_F(StageBindingRender, ConstantAndSetAtOtherPlacesInEachStage)
{
	UseStages(LoadShader(Shader::ShaderType::Vertex, ConstantFirstTest_Vertex, ConstantFirstTest_Vertex_size, ConstantFirstTest_Vertex_reflection,
	                     ConstantFirstTest_Vertex_reflection_size),
	          LoadShader(Shader::ShaderType::Fragment, SetFirstTest_Fragment, SetFirstTest_Fragment_size, SetFirstTest_Fragment_reflection,
	                     SetFirstTest_Fragment_reflection_size));

	DrawMovedQuad(Red);

	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Background));
}

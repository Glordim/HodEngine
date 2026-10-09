#include <gtest/gtest.h>

#include "RenderTestHelpers.hpp"
#include "VisualMode.hpp"

#include <HodEngine/Core/Memory/DefaultAllocator.hpp>
#include <HodEngine/Core/StaticArray.hpp>
#include <HodEngine/Core/Vector.hpp>

#include <HodEngine/Math/Color.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector2.hpp>
#include <HodEngine/Math/Vector4.hpp>

#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/MaterialManager.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/PresentationSurface.hpp>
#include <HodEngine/RHI/Texture.hpp>

#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

#include <chrono>
#include <cmath>
#include <functional>
#include <string>

using namespace hod;

// ============================================================================
// Renders through the regular path (FrameResources -> RenderView -> RenderCommandMesh)
// into a CPU readable RenderTarget, then checks pixels.
// The camera maps [-1, 1] on both axes to the whole target.
// ============================================================================

class MaterialRender : public OutputCheckedTest
{
protected:
	void SetUp() override
	{
		OutputCheckedTest::SetUp();

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
		InitRenderTarget(DefaultTargetSize);
	}

	void InitRenderTarget(uint32_t size)
	{
		Texture::CreateInfo createInfo;
		createInfo._allowReadWrite = true;

		_targetSize = size;
		ASSERT_TRUE(_renderTarget->Init(_targetSize, _targetSize, createInfo));
	}

	void TearDown() override
	{
		if (GetVisualMode()._enabled)
		{
			ShowLastFrame();
		}

		// Nothing drawn by the test may still be in flight when its resources are released
		RhiDevice::GetInstance()->WaitIdle();

		for (MaterialInstance* materialInstance : _materialInstances)
		{
			DefaultAllocator::GetInstance().Delete(materialInstance);
		}
		for (Texture* texture : _textures)
		{
			DefaultAllocator::GetInstance().Delete(texture);
		}
		DefaultAllocator::GetInstance().Delete(_renderTarget);

		OutputCheckedTest::TearDown();
	}

	MaterialInstance* CreateMaterialInstance(MaterialManager::BuiltinMaterial builtinMaterial)
	{
		MaterialInstance* materialInstance = MaterialInstance::Create(MaterialManager::GetInstance()->GetBuiltinMaterial(builtinMaterial));
		_materialInstances.PushBack(materialInstance);
		return materialInstance;
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

	void BeginFrame()
	{
		Renderer::GetInstance()->AcquireNextFrame();

		_renderView = Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
		_renderView->Init();
		_renderView->Prepare(_renderTarget, nullptr);

		Rect viewport;
		viewport._position = Vector2(0.0f, 0.0f);
		viewport._size = Vector2((float)_targetSize, (float)_targetSize);
		_renderView->SetupCamera(Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1024.0f, 1024.0f), Matrix4::Identity, viewport);

		// One texture per frame in flight: keep the one this frame renders to
		_frameTexture = _renderTarget->GetColorTexture();
	}

	void EndFrame()
	{
		Renderer::GetInstance()->Render();
		_renderView = nullptr;
	}

	// Axis aligned quad, uv (0, 0) on its (left, bottom) corner
	void DrawQuad(float left, float bottom, float right, float top, const MaterialInstance* materialInstance)
	{
		PushQuad(*_renderView, left, bottom, right, top, 0.0f, 1.0f, materialInstance);
	}

	static void PushQuad(RenderView& renderView, float left, float bottom, float right, float top, float bottomV, float topV, const MaterialInstance* materialInstance)
	{
		StaticArray<Vector2, 4> positions = {
			Vector2(left, top),
			Vector2(right, top),
			Vector2(left, bottom),
			Vector2(right, bottom),
		};

		StaticArray<Vector2, 4> uvs = {
			Vector2(0.0f, topV),
			Vector2(1.0f, topV),
			Vector2(0.0f, bottomV),
			Vector2(1.0f, bottomV),
		};

		StaticArray<uint16_t, 6> indices = {0, 1, 2, 2, 1, 3};

		renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(positions.Data(), uvs.Data(), nullptr, (uint32_t)positions.Size(), indices.Data(),
		                                                                                    (uint32_t)indices.Size(), Matrix4::Identity, materialInstance, 0, 0, true));
	}

	// Visual mode only: presents the last rendered frame in the window for VisualMode::_delayMs
	void ShowLastFrame()
	{
		VisualMode& visualMode = GetVisualMode();
		if (_frameTexture == nullptr)
		{
			return;
		}

		DesktopWindow* window = static_cast<DesktopWindow*>(visualMode._window);
		window->SetTitle(::testing::UnitTest::GetInstance()->current_test_info()->name());

		MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
		materialInstance->SetVec4("ubo.color", ToVector4(White));
		materialInstance->SetTexture("image", _frameTexture);

		PresentationSurface* presentationSurface = RhiDevice::GetInstance()->FindPresentationSurface(window);

		const auto end = std::chrono::steady_clock::now() + std::chrono::milliseconds(visualMode._delayMs);
		do
		{
			DisplayManager::GetInstance()->Update();
			if (window->IsClose())
			{
				// Window closed by the user: the remaining tests run without it
				RhiDevice::GetInstance()->DestroyPresentationSurface(window);
				visualMode._enabled = false;
				return;
			}

			Renderer::GetInstance()->AcquireNextFrame();

			FrameResources& frameResources = Renderer::GetInstance()->GetCurrentFrameResources();
			if (frameResources.AcquireSurface(presentationSurface))
			{
				Rect viewport;
				viewport._position = Vector2(0.0f, 0.0f);
				viewport._size = Vector2((float)window->GetWidth(), (float)window->GetHeight());

				RenderView* renderView = frameResources.CreateRenderView();
				renderView->Init();
				renderView->Prepare(presentationSurface);
				renderView->SetupCamera(Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1024.0f, 1024.0f), Matrix4::Identity, viewport);

				// The first row of the target is its top
				PushQuad(*renderView, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, materialInstance);
			}

			Renderer::GetInstance()->Render();
		} while (std::chrono::steady_clock::now() < end);
	}

	// Renders one frame and blocks until its content can be read back
	void RenderFrame(const std::function<void()>& draw)
	{
		BeginFrame();
		draw();
		EndFrame();
		RhiDevice::GetInstance()->WaitIdle();
	}

	// Pixel of the last rendered frame; x and y in [-1, 1], same space as DrawQuad
	Color ReadPixel(float x, float y) const
	{
		// The viewport is flipped by the RHI so that +y goes up: row 0 is the top of the target
		float pixelX = (x * 0.5f + 0.5f) * (float)_targetSize;
		float pixelY = (0.5f - y * 0.5f) * (float)_targetSize;
		return _frameTexture->ReadPixel(Vector2(pixelX, pixelY));
	}

protected:
	RenderTarget* _renderTarget = nullptr;
	uint32_t      _targetSize = 0;
	RenderView*   _renderView = nullptr;
	Texture*      _frameTexture = nullptr;

	Vector<MaterialInstance*> _materialInstances;
	Vector<Texture*>          _textures;
};

// ============================================================================
// Uniforms
// ============================================================================

TEST_F(MaterialRender, ClearColor)
{
	RenderFrame([]() {});

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Background));
}

TEST_F(MaterialRender, UniformColor)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));

	RenderFrame([&]() { DrawQuad(-0.5f, -0.5f, 0.5f, 0.5f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.75f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, -0.75f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.75f), Background));
}

TEST_F(MaterialRender, TwoInstancesOfTheSameMaterialKeepTheirOwnValues)
{
	MaterialInstance* redInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
	redInstance->SetVec4("ubo.color", ToVector4(Red));

	MaterialInstance* greenInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
	greenInstance->SetVec4("ubo.color", ToVector4(Green));

	RenderFrame([&]() {
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f, redInstance);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f, greenInstance);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Green));
}

TEST_F(MaterialRender, UniformChangedBetweenFramesIsSeenByTheNextFrame)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);

	materialInstance->SetVec4("ubo.color", ToVector4(Red));
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));

	materialInstance->SetVec4("ubo.color", ToVector4(Green));
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));

	// Unchanged value, drawn on the frame slot that last held Red
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

// Frames are submitted back to back, without waiting for the GPU: the value written for frame N + 1
// must not replace what frame N, possibly still in flight, reads.
// Frame N is read back once its frame slot comes around again, the way picking does.
// This is a race between the GPU executing frame N and the CPU recording frame N + 1, so each frame is
// kept as cheap as possible for the CPU and many of them are submitted: a broken versioning then fails
// the test nearly every run, but not provably every run.
TEST_F(MaterialRender, FramesInFlightKeepTheValueTheyWereSubmittedWith)
{
	const StaticArray<Color, 4> colors = {Red, Green, Blue, Yellow};
	const uint32_t              frameInFlightCount = RhiDevice::GetInstance()->GetFrameInFlightCount();
	const uint32_t              frameCount = 1024;

	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);

	for (uint32_t frame = 0; frame < frameCount + frameInFlightCount; ++frame)
	{
		BeginFrame(); // waits for the frame previously rendered on this slot

		if (frame >= frameInFlightCount)
		{
			uint32_t renderedFrame = frame - frameInFlightCount;
			EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), colors[renderedFrame % colors.Size()])) << "frame " << renderedFrame;
		}

		materialInstance->SetVec4("ubo.color", ToVector4(colors[frame % colors.Size()]));
		DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance);

		EndFrame();
	}
}

// ============================================================================
// Textures
// ============================================================================

TEST_F(MaterialRender, Texture)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetTexture("image", CreateFourColorsTexture());

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	// uv (0, 0), the first texel, is on the (left, bottom) corner of the quad
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, -0.5f), Green));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.5f), Blue));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.5f), Yellow));
}

TEST_F(MaterialRender, TextureIsModulatedByTheUniformColor)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));
	materialInstance->SetTexture("image", CreateFourColorsTexture());

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));                         // Red * Red
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, -0.5f), Black)); // Green * Red
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.5f), Red));                           // Yellow * Red
}

TEST_F(MaterialRender, TwoInstancesKeepTheirOwnTexture)
{
	const uint8_t bluePixels[4 * 2 * 2] = {0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255};

	Texture* blueTexture = RhiDevice::GetInstance()->CreateTexture();
	ASSERT_TRUE(blueTexture->BuildBuffer(2, 2, bluePixels, Texture::CreateInfo()));
	_textures.PushBack(blueTexture);

	MaterialInstance* fourColorsInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	fourColorsInstance->SetVec4("ubo.color", ToVector4(White));
	fourColorsInstance->SetTexture("image", CreateFourColorsTexture());

	MaterialInstance* blueInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	blueInstance->SetVec4("ubo.color", ToVector4(White));
	blueInstance->SetTexture("image", blueTexture);

	RenderFrame([&]() {
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f, fourColorsInstance);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f, blueInstance);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Blue));
}

TEST_F(MaterialRender, TextureChangedBetweenFramesIsSeenByTheNextFrame)
{
	const uint8_t bluePixels[4 * 2 * 2] = {0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255};

	Texture* blueTexture = RhiDevice::GetInstance()->CreateTexture();
	ASSERT_TRUE(blueTexture->BuildBuffer(2, 2, bluePixels, Texture::CreateInfo()));
	_textures.PushBack(blueTexture);

	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));

	materialInstance->SetTexture("image", CreateFourColorsTexture());
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));

	materialInstance->SetTexture("image", blueTexture);
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Blue));

	// Unchanged texture, drawn on the frame slot that last held the four colors one
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Blue));
}

// ============================================================================
// Missing textures
// ============================================================================

// An explicit null texture is a supported way to draw with the uniform color only
TEST_F(MaterialRender, NullTextureRendersTheUniformColor)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(Green));
	materialInstance->SetTexture("image", nullptr);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

// A texture slot nobody set must not sample an unbound slot: the fallback texture is used, and reported
TEST_F(MaterialRender, NeverSetTextureFallsBackAndIsReported)
{
	// The report is emitted once per material: use a material no other test leaves a texture unset on
	MaterialInstance* materialInstance = CreateMaterialInstance(MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
	materialInstance->SetVec4("ubo.color", ToVector4(White));

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), FallbackColor));

	_expectedWarningCount = 1;
}

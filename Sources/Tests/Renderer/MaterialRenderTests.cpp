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
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/ReadbackRenderTarget.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/RenderView.hpp>

#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Sampler.hpp>
#include <HodEngine/RHI/PresentationSurface.hpp>
#include <HodEngine/RHI/Texture.hpp>
#include <HodEngine/Renderer/Text/GlyphCache.hpp>
#include <HodEngine/Renderer/Text/TextMesh.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextLayout.hpp>
#include <HodEngine/Text/TextShaper.hpp>

#include <HodEngine/Window/Desktop/DesktopWindow.hpp>
#include <HodEngine/Window/DisplayManager.hpp>

#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iterator>
#include <string>
#include <vector>

using namespace hod;

// ============================================================================
// Renders through the regular path (FrameResources -> RenderView -> RenderCommandMesh)
// into a ReadbackRenderTarget, then checks pixels.
// The camera maps [-1, 1] on both axes to the whole target.
// ============================================================================

class MaterialRender : public OutputCheckedTest
{
protected:
	void SetUp() override
	{
		OutputCheckedTest::SetUp();

		_targetSize = DefaultTargetSize;
		ASSERT_TRUE(_renderTarget.Init(_targetSize, _targetSize));
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
		_renderTarget.Clear();

		OutputCheckedTest::TearDown();
	}

	MaterialInstance* CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial builtinMaterial)
	{
		MaterialInstance* materialInstance = MaterialInstance::Create(BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(builtinMaterial));
		_materialInstances.PushBack(materialInstance);
		return materialInstance;
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

	void BeginFrame()
	{
		Renderer::GetInstance()->AcquireNextFrame();

		_renderView = Renderer::GetInstance()->GetCurrentFrameResources().CreateRenderView();
		_renderView->Init();
		_renderView->Prepare(_renderTarget.GetRenderTarget(), nullptr);

		Rect viewport;
		viewport._position = Vector2(0.0f, 0.0f);
		viewport._size = Vector2((float)_targetSize, (float)_targetSize);
		_renderView->SetupCamera(_projection, _cameraTransform, viewport);

		// One target per frame in flight: keep the texture this frame renders to
		_frameTexture = _renderTarget.GetColorTexture();
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

		MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
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
	// By default the camera sits at the origin and sees [-1, 1] on both axes
	Matrix4 _projection = Matrix4::OrthogonalProjection(-1.0f, 1.0f, -1.0f, 1.0f, -1024.0f, 1024.0f);
	Matrix4 _cameraTransform = Matrix4::Identity;

	ReadbackRenderTarget _renderTarget;
	uint32_t             _targetSize = 0;
	RenderView*          _renderView = nullptr;
	Texture*             _frameTexture = nullptr;

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
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));

	RenderFrame([&]() { DrawQuad(-0.5f, -0.5f, 0.5f, 0.5f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.75f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, -0.75f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.75f), Background));
}

// The picking pass writes ids with it: what it draws replaces the target, alpha included
TEST_F(MaterialRender, OpaqueBuiltinReplacesWhatIsUnder)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle_Opaque);
	materialInstance->SetVec4("ubo.color", Vector4(1.0f, 0.0f, 0.0f, 0.5f));

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f, 0.5f)));
}

// The default camera maps world coordinates straight to the target, which would hide a camera that is not applied
TEST_F(MaterialRender, CameraPlacesTheGeometry)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));

	// Sees [-2, 2] on both axes, from one unit to the right: the target spans the world from x = -1 to x = 3
	_projection = Matrix4::OrthogonalProjection(-2.0f, 2.0f, -2.0f, 2.0f, -1024.0f, 1024.0f);
	_cameraTransform = Matrix4::Translation(Vector2(1.0f, 0.0f));

	// World x in [1, 2] lands on the third quarter of the target
	RenderFrame([&]() { DrawQuad(1.0f, -2.0f, 2.0f, 2.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(-0.25f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.25f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.75f, 0.0f), Background));
}

TEST_F(MaterialRender, TwoInstancesOfTheSameMaterialKeepTheirOwnValues)
{
	MaterialInstance* redInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
	redInstance->SetVec4("ubo.color", ToVector4(Red));

	MaterialInstance* greenInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);
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
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);

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
// Frame N is read back once its frame slot comes around again, which is what a ReadbackRenderTarget is for.
// This is a race between the GPU executing frame N and the CPU recording frame N + 1, so each frame is
// kept as cheap as possible for the CPU and many of them are submitted: a broken versioning then fails
// the test nearly every run, but not provably every run.
TEST_F(MaterialRender, FramesInFlightKeepTheValueTheyWereSubmittedWith)
{
	const StaticArray<Color, 4> colors = {Red, Green, Blue, Yellow};
	const uint32_t              frameInFlightCount = RhiDevice::GetInstance()->GetFrameInFlightCount();
	const uint32_t              frameCount = 1024;

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2f_Unlit_Triangle);

	for (uint32_t frame = 0; frame < frameCount + frameInFlightCount; ++frame)
	{
		BeginFrame(); // waits for the frame previously rendered on this slot

		if (frame >= frameInFlightCount)
		{
			uint32_t renderedFrame = frame - frameInFlightCount;
			float    center = (float)_targetSize * 0.5f;
			EXPECT_TRUE(ColorNear(_renderTarget.ReadPixel(Vector2(center, center)), colors[renderedFrame % colors.Size()])) << "frame " << renderedFrame;
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
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
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
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
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
	ASSERT_TRUE(blueTexture->Build(TextureCreateInfo(2, 2), bluePixels));
	_textures.PushBack(blueTexture);

	MaterialInstance* fourColorsInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	fourColorsInstance->SetVec4("ubo.color", ToVector4(White));
	fourColorsInstance->SetTexture("image", CreateFourColorsTexture());

	MaterialInstance* blueInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	blueInstance->SetVec4("ubo.color", ToVector4(White));
	blueInstance->SetTexture("image", blueTexture);

	RenderFrame([&]() {
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f, fourColorsInstance);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f, blueInstance);
	});

	EXPECT_TRUE(ColorNear(ReadPixel(-0.75f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Blue));
}

// SetTexture comes with the default sampler of the texture, SetSampler replaces it
TEST_F(MaterialRender, SamplerCanBeSetApartFromTheTexture)
{
	Sampler::CreateInfo linear;
	linear._filterMode = FilterMode::Linear;

	Texture* texture = CreateFourColorsTexture(); // its default sampler is a nearest one

	MaterialInstance* nearestInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	nearestInstance->SetVec4("ubo.color", ToVector4(White));
	nearestInstance->SetTexture("image", texture);

	MaterialInstance* linearInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	linearInstance->SetVec4("ubo.color", ToVector4(White));
	linearInstance->SetTexture("image", texture);
	linearInstance->SetSampler("imageSampler", RhiDevice::GetInstance()->GetSampler(linear));

	RenderFrame([&]() {
		DrawQuad(-1.0f, -1.0f, 0.0f, 1.0f, nearestInstance);
		DrawQuad(0.0f, -1.0f, 1.0f, 1.0f, linearInstance);
	});

	// Just past the center of each quad, towards its (right, top) texel
	EXPECT_TRUE(ColorNear(ReadPixel(-0.47f, 0.06f), Yellow));
	EXPECT_TRUE(IsBlended(ReadPixel(0.53f, 0.06f)));
}

// The sampler set by SetSampler is kept by the next SetTexture, until SetSampler(nullptr) goes back to the default one of the texture
TEST_F(MaterialRender, SamplerSetApartIsKeptBySetTexture)
{
	Sampler::CreateInfo linear;
	linear._filterMode = FilterMode::Linear;

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetSampler("imageSampler", RhiDevice::GetInstance()->GetSampler(linear));
	materialInstance->SetTexture("image", CreateFourColorsTexture()); // its default sampler is a nearest one

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(IsBlended(ReadPixel(0.06f, 0.12f)));

	materialInstance->SetSampler("imageSampler", nullptr);
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.06f, 0.12f), Yellow));
}

// The default sampler of a texture replaced after SetTexture (live edit of its settings) is the one drawn with
TEST_F(MaterialRender, DefaultSamplerChangedAfterSetTextureIsFollowed)
{
	Sampler::CreateInfo linear;
	linear._filterMode = FilterMode::Linear;

	Texture* texture = CreateFourColorsTexture(); // its default sampler is a nearest one

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetTexture("image", texture);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(0.06f, 0.12f), Yellow));

	texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(linear));
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(IsBlended(ReadPixel(0.06f, 0.12f)));
}

TEST_F(MaterialRender, TextureChangedBetweenFramesIsSeenByTheNextFrame)
{
	const uint8_t bluePixels[4 * 2 * 2] = {0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255, 0, 0, 255, 255};

	Texture* blueTexture = RhiDevice::GetInstance()->CreateTexture();
	ASSERT_TRUE(blueTexture->Build(TextureCreateInfo(2, 2), bluePixels));
	_textures.PushBack(blueTexture);

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
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
// Texture regions and formats
// ============================================================================

TEST_F(MaterialRender, UpdateRegionReplacesOnlyThatRegion)
{
	const uint8_t whitePixel[4] = {255, 255, 255, 255};

	Texture* texture = CreateFourColorsTexture();
	ASSERT_TRUE(texture->UpdateRegion(1, 0, 1, 1, whitePixel)); // Green

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetTexture("image", texture);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, -0.5f), White));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.5f), Blue));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.5f), Yellow));
}

// The texture was drawn by the frames before: the copy waits for them, the next frame sees it
TEST_F(MaterialRender, UpdateRegionBetweenFramesIsSeenByTheNextFrame)
{
	const uint8_t whitePixel[4] = {255, 255, 255, 255};

	Texture* texture = CreateFourColorsTexture();

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetTexture("image", texture);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));

	ASSERT_TRUE(texture->UpdateRegion(0, 0, 1, 1, whitePixel)); // Red
	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), White));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, -0.5f), Green));
}

// Built without pixels then filled region by region, as a font atlas is
TEST_F(MaterialRender, R8TextureIsSampledAsRed)
{
	const uint8_t pixels[2 * 2] = {255, 0, 0, 255};

	Sampler::CreateInfo nearest;
	nearest._filterMode = FilterMode::Nearest;

	Texture::CreateInfo createInfo = TextureCreateInfo(2, 2);
	createInfo._format = PixelFormat::R8_UNorm;

	Texture* texture = RhiDevice::GetInstance()->CreateTexture();
	_textures.PushBack(texture);
	ASSERT_TRUE(texture->Build(createInfo));
	texture->SetDefaultSampler(RhiDevice::GetInstance()->GetSampler(nearest));
	EXPECT_EQ(texture->GetFormat(), PixelFormat::R8_UNorm);

	ASSERT_TRUE(texture->UpdateRegion(0, 0, 2, 2, pixels));

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetTexture("image", texture);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, -0.5f), Black));
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, 0.5f), Black));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.5f), Red));
}

TEST_F(MaterialRender, UpdateRegionRejectsWhatItCannotCopy)
{
	const uint8_t pixels[4 * 2 * 2] = {};

	Texture* texture = CreateFourColorsTexture();
	EXPECT_FALSE(texture->UpdateRegion(1, 1, 2, 1, pixels)); // out of the texture
	EXPECT_FALSE(texture->UpdateRegion(0, 0, 1, 1, pixels, 1)); // no such level

	// Drawn into, not CopyDst
	EXPECT_FALSE(_renderTarget.GetColorTexture()->UpdateRegion(0, 0, 1, 1, pixels));

	_expectedErrorCount = 3;
}

// A build refused leaves the texture as it was
TEST_F(MaterialRender, BuildRejectsUnsupportedCombinations)
{
	const uint8_t pixels[4 * 2 * 2] = {};

	Texture*       texture = CreateFourColorsTexture();
	const uint32_t generation = texture->GetGeneration();

	EXPECT_FALSE(texture->Build(TextureCreateInfo(0, 2))); // empty

	Texture::CreateInfo createInfo = TextureCreateInfo(2, 2);
	createInfo._format = PixelFormat::D16_UNorm; // CopyDst depth
	EXPECT_FALSE(texture->Build(createInfo));

	createInfo = TextureCreateInfo(2, 2);
	createInfo._usage = TextureUsage::CopyDst; // never sampled
	EXPECT_FALSE(texture->Build(createInfo));

	createInfo = TextureCreateInfo(2, 2);
	createInfo._format = PixelFormat::R8_UNorm;
	createInfo._usage = TextureUsage::Sampled | TextureUsage::RenderTarget; // R8 render target
	EXPECT_FALSE(texture->Build(createInfo));

	createInfo._format = PixelFormat::R8G8B8A8_UNorm;
	EXPECT_FALSE(texture->Build(createInfo, pixels)); // pixels for a texture drawn into

	_expectedErrorCount = 5;

	EXPECT_EQ(texture->GetGeneration(), generation);
	EXPECT_TRUE(texture->HasUsage(TextureUsage::Sampled | TextureUsage::CopyDst));

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(White));
	materialInstance->SetTexture("image", texture);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });
	EXPECT_TRUE(ColorNear(ReadPixel(-0.5f, -0.5f), Red));
}

// ============================================================================
// Missing textures
// ============================================================================

// An explicit null texture is a supported way to draw with the uniform color only
TEST_F(MaterialRender, NullTextureRendersTheUniformColor)
{
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit);
	materialInstance->SetVec4("ubo.color", ToVector4(Green));
	materialInstance->SetTexture("image", nullptr);

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Green));
}

// A texture slot nobody set must not sample an unbound slot: the fallback texture is used, and reported
TEST_F(MaterialRender, NeverSetTextureFallsBackAndIsReported)
{
	// The report is emitted once per material: use a material no other test leaves a texture unset on
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
	materialInstance->SetVec4("ubo.color", ToVector4(White));

	RenderFrame([&]() { DrawQuad(-1.0f, -1.0f, 1.0f, 1.0f, materialInstance); });

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), FallbackColor));

	_expectedWarningCount = 1;
}

// ============================================================================
// Text
// ============================================================================

// An "I" in bitmap glyphs, through TextMesh and the P2fT2f_Text_Bitmap material: its stem has the color, beside it is the background
TEST_F(MaterialRender, TextBitmapDrawsTheCoverageOfTheGlyphs)
{
	std::ifstream     file(std::string(TESTS_TEXT_FIXTURES_DIR) + "/Fonts/NotoSans-Regular.ttf", std::ios::binary);
	std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	FontFace          face;
	ASSERT_TRUE(face.LoadFromMemory(data.data(), (uint32_t)data.size()));

	// One unit is one pixel of the target
	const float halfSize = (float)_targetSize * 0.5f;
	_projection = Matrix4::OrthogonalProjection(-halfSize, halfSize, -halfSize, halfSize, -1024.0f, 1024.0f);

	constexpr float fontSize = 48.0f;
	const FontFace* faces[] = {&face};
	TextShaper      shaper;
	TextLayout      layout;
	ASSERT_TRUE(shaper.Shape("I", faces, fontSize, layout));

	GlyphCache glyphCache;
	ASSERT_TRUE(glyphCache.Init(face));

	// The I first, then enough glyphs for the page to grow: the I drawn is the one copied into the larger page
	ASSERT_NE(glyphCache.GetGlyph(layout._glyphs[0]._glyphId, GlyphCache::Mode::Bitmap, (uint32_t)fontSize), nullptr);
	for (uint32_t glyphId = 1; glyphId < 2000 && glyphCache.GetPageSize(0) == GlyphCache::InitialPageSize; ++glyphId)
	{
		ASSERT_NE(glyphCache.GetGlyph(glyphId, GlyphCache::Mode::Bitmap, (uint32_t)fontSize), nullptr);
	}
	ASSERT_GT(glyphCache.GetPageSize(0), GlyphCache::InitialPageSize);

	// Centered: the capital is about 0.7 em high
	TextMesh::Font fonts[] = {{&glyphCache, GlyphCache::Mode::Bitmap}};
	TextMesh       mesh;
	ASSERT_TRUE(mesh.Build(layout, fonts, fontSize, Vector2(-layout._width * 0.5f, -fontSize * 0.35f)));
	ASSERT_EQ(mesh._batches.Size(), 1u);

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));
	materialInstance->SetTexture("image", glyphCache.GetPage(mesh._batches[0]._pageIndex));

	RenderFrame(
		[&]()
		{
			const TextMesh::Batch& batch = mesh._batches[0];
			_renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(batch._positions.Data(), batch._uvs.Data(), nullptr,
			                                                                                     batch._positions.Size(), batch._indices.Data(), batch._indices.Size(),
			                                                                                     Matrix4::Identity, materialInstance, 0, 0, true));
		});

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.4f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.8f), Background)); // above the capital

	RhiDevice::GetInstance()->WaitIdle();
	glyphCache.Clear();
}

// An "I" in Sdf glyphs, drawn larger than they are rasterized at, through TextMesh::GetMaterial (P2fT2f_Text_Sdf): its stem has the color, beside it is the background,
// and its edge is smoothed over about a pixel, not stretched over the texels magnified
TEST_F(MaterialRender, TextSdfDrawsTheInsideOfTheOutline)
{
	std::ifstream     file(std::string(TESTS_TEXT_FIXTURES_DIR) + "/Fonts/NotoSans-Regular.ttf", std::ios::binary);
	std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	FontFace          face;
	ASSERT_TRUE(face.LoadFromMemory(data.data(), (uint32_t)data.size()));

	// One unit is one pixel of the target
	const float halfSize = (float)_targetSize * 0.5f;
	_projection = Matrix4::OrthogonalProjection(-halfSize, halfSize, -halfSize, halfSize, -1024.0f, 1024.0f);

	constexpr float fontSize = 1.25f * GlyphCache::SdfEmSize; // in the target, magnified all the same
	const FontFace* faces[] = {&face};
	TextShaper      shaper;
	TextLayout      layout;
	ASSERT_TRUE(shaper.Shape("I", faces, fontSize, layout));

	GlyphCache glyphCache;
	ASSERT_TRUE(glyphCache.Init(face));

	// Centered: the capital is about 0.7 em high
	TextMesh::Font fonts[] = {{&glyphCache, GlyphCache::Mode::Sdf}};
	TextMesh       mesh;
	ASSERT_TRUE(mesh.Build(layout, fonts, fontSize, Vector2(-layout._width * 0.5f, -fontSize * 0.35f)));
	ASSERT_EQ(mesh._batches.Size(), 1u);
	ASSERT_EQ(mesh._batches[0]._mode, GlyphCache::Mode::Sdf);

	ASSERT_EQ(TextMesh::GetMaterial(GlyphCache::Mode::Sdf), BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Sdf));
	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Sdf);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));
	materialInstance->SetTexture("image", glyphCache.GetPage(mesh._batches[0]._pageIndex));

	RenderFrame(
		[&]()
		{
			const TextMesh::Batch& batch = mesh._batches[0];
			_renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(batch._positions.Data(), batch._uvs.Data(), nullptr,
			                                                                                     batch._positions.Size(), batch._indices.Data(), batch._indices.Size(),
			                                                                                     Matrix4::Identity, materialInstance, 0, 0, true));
		});

	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.0f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.4f), Red));
	EXPECT_TRUE(ColorNear(ReadPixel(0.5f, 0.0f), Background));
	EXPECT_TRUE(ColorNear(ReadPixel(0.0f, 0.8f), Background)); // above the capital

	// Across the middle row: the pixels between the color and the background are the edges of the stem, a couple on each side
	uint32_t       blendedPixelCount = 0;
	const uint32_t row = _targetSize / 2;
	for (uint32_t x = 0; x < _targetSize; ++x)
	{
		const Color pixel = _frameTexture->ReadPixel(Vector2((float)x, (float)row));
		if (ColorNear(pixel, Red) == false && ColorNear(pixel, Background) == false)
		{
			++blendedPixelCount;
		}
	}
	EXPECT_LE(blendedPixelCount, 4u);

	RhiDevice::GetInstance()->WaitIdle();
	glyphCache.Clear();
}

// Silkscreen at 4 times its grid of 8 pixels, Pixel glyphs and a Nearest sampler: every pixel of the target is the color or the background, none between
TEST_F(MaterialRender, TextPixelHasNoPixelBetweenTheColorAndTheBackground)
{
	std::ifstream     file(std::string(TESTS_TEXT_FIXTURES_DIR) + "/Fonts/Silkscreen-Regular.ttf", std::ios::binary);
	std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	FontFace          face;
	ASSERT_TRUE(face.LoadFromMemory(data.data(), (uint32_t)data.size()));

	// One unit is one pixel of the target
	const float halfSize = (float)_targetSize * 0.5f;
	_projection = Matrix4::OrthogonalProjection(-halfSize, halfSize, -halfSize, halfSize, -1024.0f, 1024.0f);

	constexpr uint32_t gridSize = 8;
	constexpr float    fontSize = 4.0f * gridSize;
	const FontFace*    faces[] = {&face};
	TextShaper         shaper;
	TextLayout         layout;
	ASSERT_TRUE(shaper.Shape("Hi", faces, fontSize, layout));

	GlyphCache glyphCache;
	ASSERT_TRUE(glyphCache.Init(face));

	TextMesh::Font fonts[] = {{&glyphCache, GlyphCache::Mode::Pixel, gridSize}};
	TextMesh       mesh;
	ASSERT_TRUE(mesh.Build(layout, fonts, fontSize, Vector2(-std::round(layout._width * 0.5f), -12.0f)));
	ASSERT_EQ(mesh._batches.Size(), 1u);

	Sampler::CreateInfo nearest;
	nearest._filterMode = FilterMode::Nearest;

	MaterialInstance* materialInstance = CreateMaterialInstance(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);
	materialInstance->SetVec4("ubo.color", ToVector4(Red));
	materialInstance->SetTexture("image", glyphCache.GetPage(mesh._batches[0]._pageIndex));
	materialInstance->SetSampler("imageSampler", RhiDevice::GetInstance()->GetSampler(nearest));

	RenderFrame(
		[&]()
		{
			const TextMesh::Batch& batch = mesh._batches[0];
			_renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(batch._positions.Data(), batch._uvs.Data(), nullptr,
			                                                                                     batch._positions.Size(), batch._indices.Data(), batch._indices.Size(),
			                                                                                     Matrix4::Identity, materialInstance, 0, 0, true));
		});

	uint32_t colorPixelCount = 0;
	uint32_t blendedPixelCount = 0;
	for (uint32_t y = 0; y < _targetSize; ++y)
	{
		for (uint32_t x = 0; x < _targetSize; ++x)
		{
			const Color pixel = _frameTexture->ReadPixel(Vector2((float)x, (float)y));
			if (ColorNear(pixel, Red))
			{
				++colorPixelCount;
			}
			else if (ColorNear(pixel, Background) == false)
			{
				++blendedPixelCount;
			}
		}
	}
	EXPECT_GT(colorPixelCount, 0u);
	EXPECT_EQ(blendedPixelCount, 0u);

	RhiDevice::GetInstance()->WaitIdle();
	glyphCache.Clear();
}

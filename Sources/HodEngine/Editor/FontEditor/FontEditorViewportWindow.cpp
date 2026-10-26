#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorViewportWindow.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/FontEditor/FontEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Math/Matrix4.hpp>
#include <HodEngine/Math/Rect.hpp>
#include <HodEngine/Math/Vector4.hpp>
#include <HodEngine/Renderer/BuiltinMaterialManager.hpp>
#include <HodEngine/Renderer/FrameResources.hpp>
#include <HodEngine/Renderer/MaterialInstance.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RenderView.hpp>
#include <HodEngine/Renderer/Renderer.hpp>
#include <HodEngine/Renderer/Resource/FontResource.hpp>
#include <HodEngine/Renderer/Text/GlyphCache.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Sampler.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextShaper.hpp>

#include <cmath>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontEditorViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	FontEditorViewportWindow::FontEditorViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);
		SetTitle("Viewport");

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
	}

	/// @brief
	FontEditorViewportWindow::~FontEditorViewportWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief
	bool FontEditorViewportWindow::Draw()
	{
		bool open = true;
		PushDockTabStyle();
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		bool visible = ImGui::Begin(GetIdentifier(), &open, GetFlags());
		ImGui::PopStyleVar(2);
		PopDockTabStyle();
		if (visible)
		{
			DrawDockTabContextMenu();
			DrawContent();
			DrawHelper::DrawHiddenTabBarButton();
		}
		if (open == false)
		{
			Close();
		}
		ImGui::End();
		return open;
	}

	/// @brief The sizes a text is often drawn at, or the first multiples of the grid of a pixel art font
	/// @param font
	/// @param text
	void FontEditorViewportWindow::BuildLines(const FontResource& font, const char* text)
	{
		_lines.Clear();

		const FontFace& face = font.GetFontFace();
		const FontFace* faces[] = {&face};
		_pixelArt = font.GetRenderMode() == FontResource::RenderMode::PixelArt && font.GetPixelSize() != 0;

		Vector<float> fontSizes;
		if (_pixelArt)
		{
			for (uint32_t multiple = 1; multiple <= 6; ++multiple)
			{
				fontSizes.PushBack((float)(font.GetPixelSize() * multiple));
			}
		}
		else
		{
			for (float fontSize : {12.0f, 16.0f, 24.0f, 32.0f, 48.0f, 72.0f})
			{
				fontSizes.PushBack(fontSize);
			}
		}

		TextShaper*            textShaper = Renderer::GetInstance()->GetTextShaper();
		GlyphCache*            glyphCache = Renderer::GetInstance()->GetGlyphCache();
		const GlyphCache::Mode mode = _pixelArt ? GlyphCache::Mode::Pixel : GlyphCache::Mode::Bitmap;

		float top = 0.0f;
		for (float fontSize : fontSizes)
		{
			PreviewLine& line = _lines.EmplaceBack();
			line._fontSize = fontSize;
			line._label = _pixelArt ? String::Format("x%u  %u px", (uint32_t)fontSize / font.GetPixelSize(), (uint32_t)fontSize) : String::Format("%u px", (uint32_t)fontSize);

			if (textShaper->Shape(text, faces, fontSize, line._layout) == false)
			{
				line._layout.Clear();
			}

			// On whole pixels: the glyphs are, from it
			line._baseline = std::round(top - line._layout._ascender);
			line._xHeight = (float)face.GetXHeight() * fontSize / (float)face.GetUnitsPerEm();
			line._mesh.Build(line._layout, faces, fontSize, Vector2(LabelColumnWidth, line._baseline), *glyphCache, mode, font.GetPixelSize());

			top = line._baseline + line._layout._descender - line._layout._lineGap - LineSpacing;
		}
	}

	/// @brief The wheel zooms around the cursor, a drag pans
	/// @param viewportPosition
	/// @param viewportSize
	void FontEditorViewportWindow::HandleInput(const Vector2& viewportPosition, const Vector2& viewportSize)
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		const ImGuiIO& io = ImGui::GetIO();

		ImGui::SetCursorScreenPos(ImVec2(viewportPosition.GetX(), viewportPosition.GetY()));
		ImGui::InvisibleButton("##Viewport", ImVec2(viewportSize.GetX(), viewportSize.GetY()), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);

		Vector2 panOffset = fontEditorTab->GetPanOffset();
		if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)))
		{
			panOffset = panOffset + Vector2(io.MouseDelta.x, -io.MouseDelta.y); // y up
			fontEditorTab->SetPanOffset(panOffset);
		}

		if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
		{
			// The point of the text under the cursor stays under it
			const float   zoomFactor = fontEditorTab->GetZoomFactor();
			const Vector2 cursor(io.MousePos.x - viewportPosition.GetX(), -(io.MousePos.y - viewportPosition.GetY()));
			const Vector2 pointUnderCursor = (cursor - panOffset) / zoomFactor;

			fontEditorTab->SetZoomFactor(zoomFactor * std::pow(1.25f, io.MouseWheel));
			fontEditorTab->SetPanOffset(cursor - pointUnderCursor * fontEditorTab->GetZoomFactor());
		}
	}

	/// @brief For each line: its ascender, cap height, x-height, baseline and descender, over its text
	/// @param viewportPosition
	/// @param viewportSize
	/// @param font
	void FontEditorViewportWindow::DrawMetrics(const Vector2& viewportPosition, const Vector2& viewportSize, const FontResource& font)
	{
		FontEditorTab* fontEditorTab = GetOwner<FontEditorTab>();
		const float    zoomFactor = fontEditorTab->GetZoomFactor();
		const Vector2& panOffset = fontEditorTab->GetPanOffset();

		// From the space of the lines (y up) to the screen (y down)
		auto toScreen = [&](float x, float y)
		{
			return ImVec2(viewportPosition.GetX() + std::round(panOffset.GetX()) + x * zoomFactor, viewportPosition.GetY() - (std::round(panOffset.GetY()) + y * zoomFactor));
		};

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->PushClipRect(ImVec2(viewportPosition.GetX(), viewportPosition.GetY()), ImVec2(viewportPosition.GetX() + viewportSize.GetX(), viewportPosition.GetY() + viewportSize.GetY()), true);

		const ImU32 labelColor = IM_COL32(150, 150, 150, 255);
		const float unitsPerEm = (float)font.GetFontFace().GetUnitsPerEm();
		const float capHeight = (float)font.GetFontFace().GetCapHeight() / unitsPerEm;

		for (const PreviewLine& line : _lines)
		{
			const ImVec2 labelPosition = toScreen(0.0f, line._baseline);
			drawList->AddText(ImVec2(labelPosition.x, labelPosition.y - ImGui::GetTextLineHeight()), labelColor, line._label.CStr());

			if (fontEditorTab->IsShowingMetrics() == false)
			{
				continue;
			}

			struct Metric
			{
				float _height;
				ImU32 _color;
			};
			const Metric metrics[] = {
				{line._layout._ascender, IM_COL32(110, 110, 110, 200)},
				{capHeight * line._fontSize, IM_COL32(255, 150, 50, 200)},
				{line._xHeight, IM_COL32(80, 160, 255, 200)},
				{0.0f, IM_COL32(230, 230, 230, 220)},
				{line._layout._descender, IM_COL32(110, 110, 110, 200)},
			};

			const float left = LabelColumnWidth - 4.0f;
			const float right = LabelColumnWidth + Max(line._layout._width, 1.0f) + 4.0f;
			for (const Metric& metric : metrics)
			{
				// On the pixel row the height ends at: the line does not hide the one the glyph ends on
				const float y = line._baseline + std::round(metric._height);
				drawList->AddLine(toScreen(left, y), toScreen(right, y), metric._color);
			}
		}

		drawList->PopClipRect();
	}

	/// @brief
	void FontEditorViewportWindow::DrawContent()
	{
		FontEditorTab*                fontEditorTab = GetOwner<FontEditorTab>();
		std::shared_ptr<FontResource> font = fontEditorTab->GetFont();
		if (font == nullptr || font->GetFontFace().IsLoaded() == false)
		{
			ImGui::TextDisabled("The font is not loaded");
			return;
		}

		const ImVec2   available = ImGui::GetContentRegionAvail();
		const uint32_t resolutionWidth = Clamp((uint32_t)available.x, 2u, 16u * 1024u);
		const uint32_t resolutionHeight = Clamp((uint32_t)available.y, 2u, 16u * 1024u);
		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			_renderTarget->Init(resolutionWidth, resolutionHeight);
		}
		if (_renderTarget->IsValid() == false)
		{
			return;
		}

		const ImVec2  cursorPosition = ImGui::GetCursorScreenPos();
		const Vector2 viewportPosition(cursorPosition.x, cursorPosition.y);
		const Vector2 viewportSize((float)resolutionWidth, (float)resolutionHeight);

		// The input first, for the text to be drawn where it is this frame
		HandleInput(viewportPosition, viewportSize);

		// A font reloaded has another face: laid out again
		if (_builtPreviewVersion != fontEditorTab->GetPreviewVersion() || _builtFontFaceId != font->GetFontFace().GetId())
		{
			BuildLines(*font, fontEditorTab->GetPreviewText());
			_builtPreviewVersion = fontEditorTab->GetPreviewVersion();
			_builtFontFaceId = font->GetFontFace().GetId();
		}

		FrameResources& frameResources = Renderer::GetInstance()->GetCurrentFrameResources();
		RenderView*     renderView = frameResources.CreateRenderView();
		renderView->Init();
		renderView->Prepare(_renderTarget, nullptr);

		Rect viewport;
		viewport._position = Vector2(0.0f, 0.0f);
		viewport._size = viewportSize;

		// One unit per pixel, origin at the top left of the render target, y up
		const Matrix4 projection = Matrix4::OrthogonalProjection(0.0f, viewportSize.GetX(), -viewportSize.GetY(), 0.0f, -1024, 1024);
		renderView->SetupCamera(projection, Matrix4::Identity, viewport);

		// The pan on whole pixels: at 100%, the glyphs are on the pixels of the viewport
		const Vector2 panOffset = fontEditorTab->GetPanOffset();
		const float   zoomFactor = fontEditorTab->GetZoomFactor();
		const Matrix4 modelMatrix = Matrix4::Translation(Vector2(std::round(panOffset.GetX()), std::round(panOffset.GetY()))) * Matrix4::Scale(Vector2(zoomFactor, zoomFactor));

		// Magnified, the texels are shown as they are: the pixels the text is drawn with
		Sampler::CreateInfo samplerCreateInfo;
		samplerCreateInfo._filterMode = (_pixelArt || zoomFactor > 1.0f) ? FilterMode::Nearest : FilterMode::Linear;
		const Sampler*  sampler = RhiDevice::GetInstance()->GetSampler(samplerCreateInfo);
		GlyphCache*     glyphCache = Renderer::GetInstance()->GetGlyphCache();
		const Material* material = BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);

		for (const PreviewLine& line : _lines)
		{
			for (const TextMesh::Batch& batch : line._mesh._batches)
			{
				// One per frame: the one of the previous frame may still be in use by the GPU
				MaterialInstance* materialInstance = MaterialInstance::Create(material);
				materialInstance->SetTexture("image", glyphCache->GetPage(batch._pageIndex));
				materialInstance->SetSampler("imageSampler", sampler);
				materialInstance->SetVec4("ubo.color", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				frameResources.DeleteAfter(materialInstance);

				renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(batch._positions.Data(), batch._uvs.Data(), nullptr,
				                                                                                     batch._positions.Size(), batch._indices.Data(), batch._indices.Size(),
				                                                                                     modelMatrix, materialInstance, 0, 0, true));
			}
		}

		ImGui::GetWindowDrawList()->AddImage(_renderTarget->GetColorTexture(), cursorPosition, ImVec2(cursorPosition.x + viewportSize.GetX(), cursorPosition.y + viewportSize.GetY()));
		DrawMetrics(viewportPosition, viewportSize, *font);
	}
}

#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontPreviewViewportWindow.hpp"

#include "HodEngine/Editor/DrawHelper.hpp"
#include "HodEngine/Editor/FontEditor/FontPreviewTab.hpp"

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
#include <HodEngine/Renderer/Resource/FontFaceResource.hpp>
#include <HodEngine/Renderer/Text/GlyphCache.hpp>
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Sampler.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextShaper.hpp>

#include <cmath>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontPreviewViewportWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	FontPreviewViewportWindow::FontPreviewViewportWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);
		SetTitle("Viewport");

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
	}

	/// @brief
	FontPreviewViewportWindow::~FontPreviewViewportWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief
	bool FontPreviewViewportWindow::Draw()
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
	void FontPreviewViewportWindow::BuildLines(FontFaceResource& font, const char* text)
	{
		_lines.Clear();

		const FontFace& face = font.GetFontFace();
		const bool      pixelArt = font.GetGlyphMode() == GlyphCache::Mode::Pixel;

		Vector<float> fontSizes;
		if (pixelArt)
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

		TextShaper* textShaper = Renderer::GetInstance()->GetTextShaper();

		float top = 0.0f;
		for (float fontSize : fontSizes)
		{
			PreviewLine& line = _lines.EmplaceBack();
			line._fontSize = fontSize;
			line._label = pixelArt ? String::Format("x%u  %u px", (uint32_t)fontSize / font.GetPixelSize(), (uint32_t)fontSize) : String::Format("%u px", (uint32_t)fontSize);

			if (textShaper->Shape(text, _fontChain.GetFaces(), fontSize, line._layout) == false)
			{
				line._layout.Clear();
			}

			// On whole pixels: the glyphs are, from it
			line._baseline = std::round(top - line._layout._ascender);
			line._xHeight = (float)face.GetXHeight() * fontSize / (float)face.GetUnitsPerEm();
			line._mesh.Build(line._layout, _fontChain.GetMeshFonts(), fontSize, Vector2(LabelColumnWidth, line._baseline));

			top = line._baseline + line._layout._descender - line._layout._lineGap - LineSpacing;
		}
	}

	/// @brief The wheel zooms around the cursor, a drag pans
	/// @param viewportPosition
	/// @param viewportSize
	void FontPreviewViewportWindow::HandleInput(const Vector2& viewportPosition, const Vector2& viewportSize)
	{
		FontPreviewTab* fontPreviewTab = GetOwner<FontPreviewTab>();
		const ImGuiIO& io = ImGui::GetIO();

		ImGui::SetCursorScreenPos(ImVec2(viewportPosition.GetX(), viewportPosition.GetY()));
		ImGui::InvisibleButton("##Viewport", ImVec2(viewportSize.GetX(), viewportSize.GetY()), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);

		Vector2 panOffset = fontPreviewTab->GetPanOffset();
		if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)))
		{
			panOffset = panOffset + Vector2(io.MouseDelta.x, -io.MouseDelta.y); // y up
			fontPreviewTab->SetPanOffset(panOffset);
		}

		if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
		{
			// The point of the text under the cursor stays under it
			const float   zoomFactor = fontPreviewTab->GetZoomFactor();
			const Vector2 cursor(io.MousePos.x - viewportPosition.GetX(), -(io.MousePos.y - viewportPosition.GetY()));
			const Vector2 pointUnderCursor = (cursor - panOffset) / zoomFactor;

			fontPreviewTab->SetZoomFactor(zoomFactor * std::pow(1.25f, io.MouseWheel));
			fontPreviewTab->SetPanOffset(cursor - pointUnderCursor * fontPreviewTab->GetZoomFactor());
		}
	}

	/// @brief For each line: its ascender, cap height, x-height, baseline and descender, over its text
	/// @param viewportPosition
	/// @param viewportSize
	/// @param font
	void FontPreviewViewportWindow::DrawMetrics(const Vector2& viewportPosition, const Vector2& viewportSize, const FontFaceResource& font)
	{
		FontPreviewTab* fontPreviewTab = GetOwner<FontPreviewTab>();
		const float    zoomFactor = fontPreviewTab->GetZoomFactor();
		const Vector2& panOffset = fontPreviewTab->GetPanOffset();

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

			if (fontPreviewTab->IsShowingMetrics() == false)
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
	void FontPreviewViewportWindow::DrawContent()
	{
		// Laid out again when a face of the chain changed (reloaded, drawn otherwise, added...): see FontChain
		FontPreviewTab* fontPreviewTab = GetOwner<FontPreviewTab>();
		const bool      chainChanged = fontPreviewTab->UpdateFontChain(_fontChain);
		if (_fontChain.Empty())
		{
			ImGui::TextDisabled("Nothing to draw: no face loaded");
			return;
		}
		FontFaceResource& font = *_fontChain.GetFaceResources()[0]; // the sizes are the ones of the face drawn for the style

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

		// Laid out again when the atlas of a face grew (a line, a text node...): the uvs of its glyphs changed
		bool outdated = _builtPreviewVersion != fontPreviewTab->GetPreviewVersion() || chainChanged;
		for (const PreviewLine& line : _lines)
		{
			outdated |= (outdated == false && line._mesh.IsOutdated());
		}
		// A line may make the atlas grow under the ones before it: built again then, every glyph already in the atlas
		for (uint32_t attempt = 0; outdated && attempt < 3; ++attempt)
		{
			BuildLines(font, fontPreviewTab->GetPreviewText());
			_builtPreviewVersion = fontPreviewTab->GetPreviewVersion();

			outdated = false;
			for (const PreviewLine& line : _lines)
			{
				outdated |= line._mesh.IsOutdated();
			}
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
		const Vector2 panOffset = fontPreviewTab->GetPanOffset();
		const float   zoomFactor = fontPreviewTab->GetZoomFactor();
		const Matrix4 modelMatrix = Matrix4::Translation(Vector2(std::round(panOffset.GetX()), std::round(panOffset.GetY()))) * Matrix4::Scale(Vector2(zoomFactor, zoomFactor));

		// Magnified, the texels are shown as they are: the pixels the text is drawn with. Pixel glyphs always (a fallback may be Smooth)
		Sampler::CreateInfo nearestSamplerCreateInfo;
		nearestSamplerCreateInfo._filterMode = FilterMode::Nearest;
		Sampler::CreateInfo linearSamplerCreateInfo;
		linearSamplerCreateInfo._filterMode = zoomFactor > 1.0f ? FilterMode::Nearest : FilterMode::Linear;
		const Sampler*  nearestSampler = RhiDevice::GetInstance()->GetSampler(nearestSamplerCreateInfo);
		const Sampler*  linearSampler = RhiDevice::GetInstance()->GetSampler(linearSamplerCreateInfo);
		const Material* material = BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);

		for (const PreviewLine& line : _lines)
		{
			for (const TextMesh::Batch& batch : line._mesh._batches)
			{
				// One per frame: the one of the previous frame may still be in use by the GPU
				MaterialInstance* materialInstance = MaterialInstance::Create(material);
				materialInstance->SetTexture("image", batch._glyphCache->GetPage(batch._pageIndex));
				materialInstance->SetSampler("imageSampler", batch._mode == GlyphCache::Mode::Pixel ? nearestSampler : linearSampler);
				materialInstance->SetVec4("ubo.color", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
				frameResources.DeleteAfter(materialInstance);

				renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(batch._positions.Data(), batch._uvs.Data(), nullptr,
				                                                                                     batch._positions.Size(), batch._indices.Data(), batch._indices.Size(),
				                                                                                     modelMatrix, materialInstance, 0, 0, true));
			}
		}

		ImGui::GetWindowDrawList()->AddImage(_renderTarget->GetColorTexture(), cursorPosition, ImVec2(cursorPosition.x + viewportSize.GetX(), cursorPosition.y + viewportSize.GetY()));
		DrawMetrics(viewportPosition, viewportSize, font);
	}
}

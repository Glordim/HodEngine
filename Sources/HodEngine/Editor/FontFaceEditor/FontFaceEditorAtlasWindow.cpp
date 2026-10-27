#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorAtlasWindow.hpp"

#include "HodEngine/Editor/FontFaceEditor/FontFaceEditorTab.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

#include <HodEngine/Core/Math.hpp>
#include <HodEngine/Core/StaticArray.hpp>
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

#include <cmath>

namespace hod::inline editor
{
	DESCRIBE_REFLECTED_CLASS(FontFaceEditorAtlasWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	FontFaceEditorAtlasWindow::FontFaceEditorAtlasWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);
		SetTitle("Atlas");

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
	}

	/// @brief
	FontFaceEditorAtlasWindow::~FontFaceEditorAtlasWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief The page shown, the outlines, the zoom
	/// @param pageCount
	void FontFaceEditorAtlasWindow::DrawToolbar(uint32_t pageCount)
	{
		ImGui::AlignTextToFramePadding();
		ImGui::TextUnformatted("Page");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(120.0f);
		if (ImGui::BeginCombo("##Page", String::Format("%u / %u", _pageIndex + 1, pageCount).CStr()))
		{
			for (uint32_t pageIndex = 0; pageIndex < pageCount; ++pageIndex)
			{
				if (ImGui::Selectable(String::Format("%u", pageIndex + 1).CStr(), pageIndex == _pageIndex))
				{
					_pageIndex = pageIndex;
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SameLine(0.0f, 16.0f);
		ImGui::Checkbox("Outline the glyphs", &_outlineFontGlyphs);

		ImGui::SameLine(0.0f, 16.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%u%%", static_cast<uint32_t>(_zoomFactor * 100.0f));
		ImGui::SameLine();
		if (ImGui::Button(ICON_MDI_FIT_TO_PAGE_OUTLINE " Fit"))
		{
			_fitRequested = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("1:1"))
		{
			_zoomFactor = 1.0f;
			_panOffset = Vector2(0.0f, 0.0f);
		}
	}

	/// @brief The wheel zooms around the cursor, a drag pans
	/// @param viewportPosition
	/// @param viewportSize
	void FontFaceEditorAtlasWindow::HandleInput(const Vector2& viewportPosition, const Vector2& viewportSize)
	{
		const ImGuiIO& io = ImGui::GetIO();

		ImGui::InvisibleButton("##Atlas", ImVec2(viewportSize.GetX(), viewportSize.GetY()), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonMiddle);

		if (ImGui::IsItemActive() && (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 0.0f) || ImGui::IsMouseDragging(ImGuiMouseButton_Middle, 0.0f)))
		{
			_panOffset = _panOffset + Vector2(io.MouseDelta.x, io.MouseDelta.y);
		}

		if (ImGui::IsItemHovered() && io.MouseWheel != 0.0f)
		{
			// The texel under the cursor stays under it
			const Vector2 cursorFromCenter(io.MousePos.x - viewportPosition.GetX() - viewportSize.GetX() * 0.5f, io.MousePos.y - viewportPosition.GetY() - viewportSize.GetY() * 0.5f);
			const float   previousZoomFactor = _zoomFactor;
			_zoomFactor = Clamp(_zoomFactor * std::pow(1.25f, io.MouseWheel), MinZoomFactor, MaxZoomFactor);
			const float zoomRatio = _zoomFactor / previousZoomFactor;
			_panOffset = cursorFromCenter - (cursorFromCenter - _panOffset) * zoomRatio;
		}
	}

	/// @brief
	void FontFaceEditorAtlasWindow::DrawContent()
	{
		std::shared_ptr<FontFaceResource> font = GetOwner<FontFaceEditorTab>()->GetFontFace();
		if (font == nullptr)
		{
			ImGui::TextDisabled("The font is not loaded");
			return;
		}
		GlyphCache*    glyphCache = &font->GetGlyphCache();
		const uint32_t pageCount = glyphCache->GetPageCount();

		DrawToolbar(pageCount);
		if (pageCount == 0)
		{
			ImGui::TextDisabled("No glyph rasterized yet: the atlas is filled as text is drawn");
			return;
		}
		_pageIndex = Min(_pageIndex, pageCount - 1);

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
		const float   pageSize = (float)glyphCache->GetPageSize(_pageIndex);

		if (_fitRequested)
		{
			// The largest power of two the page fits in the view at: its texels stay the same size
			_zoomFactor = Clamp(std::exp2(std::floor(std::log2(Min(viewportSize.GetX(), viewportSize.GetY()) / pageSize))), MinZoomFactor, MaxZoomFactor);
			_panOffset = Vector2(0.0f, 0.0f);
			_fitRequested = false;
		}

		HandleInput(viewportPosition, viewportSize);

		// The page, in pixels of the view from its top left (y down), its top left on a whole pixel
		const float   width = pageSize * _zoomFactor;
		const float   left = std::floor((viewportSize.GetX() - width) * 0.5f + _panOffset.GetX());
		const float   top = std::floor((viewportSize.GetY() - width) * 0.5f + _panOffset.GetY());

		// The coverage as white: the material of the text, its color white
		FrameResources&   frameResources = Renderer::GetInstance()->GetCurrentFrameResources();
		MaterialInstance* materialInstance =
			MaterialInstance::Create(BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap));
		Sampler::CreateInfo samplerCreateInfo;
		samplerCreateInfo._filterMode = _zoomFactor >= 1.0f ? FilterMode::Nearest : FilterMode::Linear;
		materialInstance->SetTexture("image", glyphCache->GetPage(_pageIndex));
		materialInstance->SetSampler("imageSampler", RhiDevice::GetInstance()->GetSampler(samplerCreateInfo));
		materialInstance->SetVec4("ubo.color", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		frameResources.DeleteAfter(materialInstance);

		RenderView* renderView = frameResources.CreateRenderView();
		renderView->Init();
		renderView->Prepare(_renderTarget, nullptr);

		Rect viewport;
		viewport._position = Vector2(0.0f, 0.0f);
		viewport._size = viewportSize;

		// One unit per pixel, origin at the top left of the render target, y up
		const Matrix4 projection = Matrix4::OrthogonalProjection(0.0f, viewportSize.GetX(), -viewportSize.GetY(), 0.0f, -1024, 1024);
		renderView->SetupCamera(projection, Matrix4::Identity, viewport);

		StaticArray<Vector2, 4> positions = {
			Vector2(left, -top),
			Vector2(left + width, -top),
			Vector2(left, -(top + width)),
			Vector2(left + width, -(top + width)),
		};
		StaticArray<Vector2, 4> uvs = {
			Vector2(0.0f, 0.0f),
			Vector2(1.0f, 0.0f),
			Vector2(0.0f, 1.0f),
			Vector2(1.0f, 1.0f),
		};
		StaticArray<uint16_t, 6> indices = {1, 0, 2, 1, 2, 3};

		renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(positions.Data(), uvs.Data(), nullptr, (uint32_t)positions.Size(), indices.Data(),
		                                                                                     (uint32_t)indices.Size(), Matrix4::Identity, materialInstance, 0, 0, true));

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddImage(_renderTarget->GetColorTexture(), cursorPosition, ImVec2(cursorPosition.x + viewportSize.GetX(), cursorPosition.y + viewportSize.GetY()));

		drawList->PushClipRect(cursorPosition, ImVec2(cursorPosition.x + viewportSize.GetX(), cursorPosition.y + viewportSize.GetY()), true);

		// The bounds of the page
		const ImVec2 pageMin(cursorPosition.x + left, cursorPosition.y + top);
		drawList->AddRect(ImVec2(pageMin.x - 1.0f, pageMin.y - 1.0f), ImVec2(pageMin.x + width + 1.0f, pageMin.y + width + 1.0f), IM_COL32(90, 90, 90, 255));

		// The glyphs in this page, their border included
		uint32_t                         pageGlyphCount = 0;
		Vector<const GlyphCache::Glyph*> glyphs;
		glyphCache->GetGlyphs(glyphs);
		for (const GlyphCache::Glyph* glyph : glyphs)
		{
			if (glyph->_empty || glyph->_pageIndex != _pageIndex)
			{
				continue;
			}
			++pageGlyphCount;

			if (_outlineFontGlyphs)
			{
				const ImVec2 glyphMin(pageMin.x + glyph->_uvMin.GetX() * width, pageMin.y + glyph->_uvMin.GetY() * width);
				const ImVec2 glyphMax(pageMin.x + glyph->_uvMax.GetX() * width, pageMin.y + glyph->_uvMax.GetY() * width);
				drawList->AddRect(glyphMin, glyphMax, IM_COL32(80, 160, 255, 200));
			}
		}

		drawList->PopClipRect();

		// What the page holds: every size and mode the font was drawn in
		const String summary = String::Format("%u glyphs in this page of %ux%u (%u pages)", pageGlyphCount, (uint32_t)pageSize, (uint32_t)pageSize, pageCount);
		drawList->AddText(ImVec2(cursorPosition.x + 6.0f, cursorPosition.y + viewportSize.GetY() - ImGui::GetTextLineHeight() - 6.0f), IM_COL32(170, 170, 170, 255), summary.CStr());
	}
}

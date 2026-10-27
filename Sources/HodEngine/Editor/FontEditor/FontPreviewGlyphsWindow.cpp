#include "HodEngine/Editor/Pch.hpp"
#include "HodEngine/Editor/FontEditor/FontPreviewGlyphsWindow.hpp"

#include "HodEngine/Editor/FontEditor/FontPreviewTab.hpp"

#include "HodEngine/Editor/Asset.hpp"
#include "HodEngine/Editor/AssetDatabase.hpp"

#include <HodEngine/ImGui/DearImGui/imgui.h>
#include <HodEngine/ImGui/Font/IconsMaterialDesignIcons.h>

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
#include <HodEngine/RHI/RenderTarget.hpp>
#include <HodEngine/RHI/RhiDevice.hpp>
#include <HodEngine/RHI/Sampler.hpp>
#include <HodEngine/Text/FontFace.hpp>
#include <HodEngine/Text/TextLayout.hpp>
#include <HodEngine/Text/Unicode.hpp>

#include <cmath>

namespace hod::inline editor
{
	namespace
	{
		struct CharacterSet
		{
			const char* _name;
			char32_t    _first; // 0 to 0: the characters of the font
			char32_t    _last;
		};

		constexpr CharacterSet CharacterSets[] = {
			{"Characters of the font", 0, 0},
			{"Basic Latin", 0x0020, 0x007E},
			{"Latin-1 Supplement", 0x00A0, 0x00FF},
			{"Latin Extended-A", 0x0100, 0x017F},
			{"Latin Extended-B", 0x0180, 0x024F},
			{"General Punctuation", 0x2000, 0x206F},
			{"Currency Symbols", 0x20A0, 0x20CF},
			{"Greek", 0x0370, 0x03FF},
			{"Cyrillic", 0x0400, 0x04FF},
			{"Hebrew", 0x0590, 0x05FF},
			{"Arabic", 0x0600, 0x06FF},
			{"Devanagari", 0x0900, 0x097F},
			{"Thai", 0x0E00, 0x0E7F},
			{"Hiragana", 0x3040, 0x309F},
			{"Katakana", 0x30A0, 0x30FF},
			{"CJK Unified Ideographs", 0x4E00, 0x9FFF},
			{"Hangul Syllables", 0xAC00, 0xD7A3},
		};

		/// @brief
		/// @param text
		/// @param value
		/// @return false when it is not hexadecimal digits only, or too many for a codepoint
		bool ParseHexadecimal(StringView text, char32_t& value)
		{
			if (text.Empty() || text.Size() > 6)
			{
				return false;
			}

			value = 0;
			for (char character : text)
			{
				uint32_t digit = 0;
				if (character >= '0' && character <= '9')
				{
					digit = (uint32_t)(character - '0');
				}
				else if (character >= 'a' && character <= 'f')
				{
					digit = (uint32_t)(character - 'a' + 10);
				}
				else if (character >= 'A' && character <= 'F')
				{
					digit = (uint32_t)(character - 'A' + 10);
				}
				else
				{
					return false;
				}
				value = value * 16 + digit;
			}
			return value <= 0x10FFFF;
		}

		/// @brief Words apart by spaces or commas. A word is a code ("U+00E9", "0x4E2D", or 4 to 6 hexadecimal digits: "4E2D"), else its characters ("é", "AB").
		/// Each character once, in the order they come
		/// @param search
		/// @param codepoints
		void ParseSearch(const char* search, Vector<char32_t>& codepoints)
		{
			auto add = [&codepoints](char32_t codepoint)
			{
				for (char32_t found : codepoints)
				{
					if (found == codepoint)
					{
						return;
					}
				}
				codepoints.PushBack(codepoint);
			};

			const StringView text(search);
			uint32_t         wordStart = 0;
			while (wordStart < text.Size())
			{
				uint32_t wordEnd = wordStart;
				while (wordEnd < text.Size() && text[wordEnd] != ' ' && text[wordEnd] != ',')
				{
					++wordEnd;
				}

				if (wordEnd > wordStart)
				{
					const StringView word(text.Data() + wordStart, wordEnd - wordStart);
					const bool       prefixed = word.Size() > 2 && (((word[0] == 'U' || word[0] == 'u') && word[1] == '+') || (word[0] == '0' && (word[1] == 'x' || word[1] == 'X')));
					const StringView digits = prefixed ? StringView(word.Data() + 2, word.Size() - 2) : word;

					char32_t codepoint = 0;
					if ((prefixed || word.Size() >= 4) && ParseHexadecimal(digits, codepoint))
					{
						add(codepoint);
					}
					else
					{
						Vector<char32_t> characters;
						DecodeUtf8(word, characters);
						for (char32_t character : characters)
						{
							add(character);
						}
					}
				}
				wordStart = wordEnd + 1;
			}
		}
	}

	DESCRIBE_REFLECTED_CLASS(FontPreviewGlyphsWindow, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief
	/// @param editorTab
	FontPreviewGlyphsWindow::FontPreviewGlyphsWindow(EditorTab* editorTab)
	: EditorTabWindow(editorTab)
	{
		SetFlags(ImGuiWindowFlags_NoScrollbar);
		SetTitle("Glyphs");

		_renderTarget = RhiDevice::GetInstance()->CreateRenderTarget();
	}

	/// @brief
	FontPreviewGlyphsWindow::~FontPreviewGlyphsWindow()
	{
		DefaultAllocator::GetInstance().Delete(_renderTarget);
	}

	/// @brief The characters of the set that are drawn (assigned, not controls nor spaces), the font has a glyph for, a fallback, or none.
	/// Or the ones searched for, whatever they are: what is searched for is shown
	void FontPreviewGlyphsWindow::UpdateCells()
	{
		_cells.Clear();
		_characterCount = 0;
		_fallbackCount = 0;
		_missingCount = 0;

		const ArrayView<const FontFace*> faces = _fontChain.GetFaces();
		const FontFace&                  face = *faces[0];
		const bool                       searching = _search[0] != '\0';
		const CharacterSet&              characterSet = CharacterSets[_characterSetIndex];
		auto                             addCharacter = [&](char32_t codepoint)
		{
			if (searching == false && IsDrawnCharacter(codepoint) == false)
			{
				return;
			}

			// The first of the chain that has it, as a text is shaped
			uint32_t glyphId = 0;
			uint32_t fontIndex = 0;
			for (uint32_t faceIndex = 0; faceIndex < faces.Size() && glyphId == 0; ++faceIndex)
			{
				glyphId = faces[faceIndex]->GetGlyphId(codepoint);
				fontIndex = faceIndex;
			}

			++_characterCount;
			if (glyphId == 0)
			{
				++_missingCount;
			}
			else if (fontIndex != 0)
			{
				++_fallbackCount;
			}
			// Missing from the font: a fallback has it or not
			if (_onlyMissing == false || glyphId == 0 || fontIndex != 0)
			{
				Cell& cell = _cells.EmplaceBack();
				cell._codepoint = codepoint;
				cell._glyphId = glyphId;
				cell._fontIndex = glyphId != 0 ? fontIndex : 0;
			}
		};

		if (searching)
		{
			Vector<char32_t> codepoints;
			ParseSearch(_search, codepoints);
			for (char32_t codepoint : codepoints)
			{
				addCharacter(codepoint);
			}
		}
		else if (characterSet._first == 0 && characterSet._last == 0)
		{
			Vector<char32_t> codepoints;
			face.GetCodepoints(codepoints);
			for (char32_t codepoint : codepoints)
			{
				addCharacter(codepoint);
			}
		}
		else
		{
			for (char32_t codepoint = characterSet._first; codepoint <= characterSet._last; ++codepoint)
			{
				addCharacter(codepoint);
			}
		}
	}

	/// @brief The set, what the font has of it, the filter
	void FontPreviewGlyphsWindow::DrawToolbar()
	{
		// The set does not count while searching: the combo says so
		const bool searching = _search[0] != '\0';
		ImGui::BeginDisabled(searching);
		ImGui::SetNextItemWidth(220.0f);
		if (ImGui::BeginCombo("##CharacterSet", searching ? "Search results" : CharacterSets[_characterSetIndex]._name))
		{
			for (uint32_t index = 0; index < std::size(CharacterSets); ++index)
			{
				if (ImGui::Selectable(CharacterSets[index]._name, index == _characterSetIndex))
				{
					_characterSetIndex = index;
				}
			}
			ImGui::EndCombo();
		}
		ImGui::EndDisabled();

		ImGui::SameLine();
		ImGui::SetNextItemWidth(220.0f);
		ImGui::InputTextWithHint("##Search", ICON_MDI_MAGNIFY " A, \xC3\xA9, U+00E9, 4E2D...", _search, sizeof(_search));
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("Characters, typed or by their code: U+00E9, 0xE9, or 4 to 6 hexadecimal digits.\nApart by spaces or commas.");
		}

		ImGui::SameLine(0.0f, 16.0f);
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%u / %u", _characterCount - _fallbackCount - _missingCount, _characterCount);
		if (_fallbackCount != 0)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.7f, 0.3f, 1.0f), "%u from fallbacks", _fallbackCount);
		}
		if (_missingCount != 0)
		{
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(1.0f, 0.45f, 0.4f, 1.0f), "%u missing", _missingCount);
		}

		ImGui::SameLine(0.0f, 16.0f);
		ImGui::Checkbox("Only the missing", &_onlyMissing);
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_ForTooltip))
		{
			ImGui::SetTooltip("The characters the font lacks, a fallback has them or not");
		}
	}

	/// @brief The grid scrolls in a child window: only its visible rows are drawn, into a render target of the size of the view
	void FontPreviewGlyphsWindow::DrawContent()
	{
		const bool chainChanged = GetOwner<FontPreviewTab>()->UpdateFontChain(_fontChain);
		if (_fontChain.Empty())
		{
			ImGui::TextDisabled("Nothing to draw: no face loaded");
			return;
		}
		const FontFace& face = *_fontChain.GetFaces()[0];

		// Again when a face of the chain changed: reloaded, added, removed...
		if (chainChanged || _builtCharacterSetIndex != _characterSetIndex || _builtOnlyMissing != _onlyMissing || _builtSearch != String(_search))
		{
			UpdateCells();
			_builtCharacterSetIndex = _characterSetIndex;
			_builtOnlyMissing = _onlyMissing;
			_builtSearch = _search;
		}

		DrawToolbar();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		const bool visible = ImGui::BeginChild("##Grid", ImVec2(0.0f, 0.0f), ImGuiChildFlags_None);
		ImGui::PopStyleVar();
		if (visible == false)
		{
			ImGui::EndChild();
			return;
		}

		const float    viewWidth = ImGui::GetContentRegionAvail().x;
		const float    viewHeight = ImGui::GetWindowSize().y;
		const uint32_t columnCount = Max(1u, (uint32_t)(viewWidth / CellSize));
		const uint32_t rowCount = (_cells.Size() + columnCount - 1) / columnCount;
		const ImVec2   viewPosition = ImGui::GetWindowPos();
		const float    scroll = ImGui::GetScrollY();

		// The extent of the whole grid, for the scrollbar
		ImGui::Dummy(ImVec2((float)columnCount * CellSize, Max(1.0f, (float)rowCount * CellSize)));

		const uint32_t resolutionWidth = Clamp((uint32_t)viewWidth, 2u, 16u * 1024u);
		const uint32_t resolutionHeight = Clamp((uint32_t)viewHeight, 2u, 16u * 1024u);
		if (_renderTarget->GetResolution().GetX() != resolutionWidth || _renderTarget->GetResolution().GetY() != resolutionHeight)
		{
			_renderTarget->Init(resolutionWidth, resolutionHeight);
		}
		if (_renderTarget->IsValid() == false || _cells.Empty())
		{
			ImGui::EndChild();
			return;
		}

		const uint32_t firstRow = (uint32_t)(scroll / CellSize);
		const uint32_t lastRow = Min(rowCount, (uint32_t)std::ceil((scroll + viewHeight) / CellSize));

		// The glyphs of the visible cells, centered in their cell on a baseline that fits the line of the font. The ones of the fallbacks at its size
		const float glyphSize = _fontChain.GetFaceResources()[0]->GetDrawnFontSize(GlyphSize);
		const float unitsToPixels = glyphSize / (float)face.GetUnitsPerEm();
		const float lineHeight = (float)(face.GetAscender() - face.GetDescender()) * unitsToPixels;
		const float baselineFromTop = std::round((CellSize - LabelHeight - lineHeight) * 0.5f + (float)face.GetAscender() * unitsToPixels);

		TextLayout layout;
		for (uint32_t row = firstRow; row < lastRow; ++row)
		{
			for (uint32_t column = 0; column < columnCount; ++column)
			{
				const uint32_t cellIndex = row * columnCount + column;
				if (cellIndex >= _cells.Size() || _cells[cellIndex]._glyphId == 0)
				{
					continue;
				}

				const Cell&     cell = _cells[cellIndex];
				const FontFace& cellFace = *_fontChain.GetFaces()[cell._fontIndex];
				const float     advance = (float)cellFace.GetGlyphAdvance(cell._glyphId) * glyphSize / (float)cellFace.GetUnitsPerEm();
				const float     cellTop = (float)row * CellSize - scroll;

				TextLayout::Glyph& glyph = layout._glyphs.EmplaceBack();
				glyph._glyphId = cell._glyphId;
				glyph._faceIndex = cell._fontIndex;
				glyph._position = Vector2((float)column * CellSize + std::round((CellSize - advance) * 0.5f), -(std::round(cellTop) + baselineFromTop)); // y up
				glyph._advance = advance;
			}
		}

		_mesh.Build(layout, _fontChain.GetMeshFonts(), glyphSize, Vector2(0.0f, 0.0f));

		FrameResources& frameResources = Renderer::GetInstance()->GetCurrentFrameResources();
		RenderView*     renderView = frameResources.CreateRenderView();
		renderView->Init();
		renderView->Prepare(_renderTarget, nullptr);

		Rect viewport;
		viewport._position = Vector2(0.0f, 0.0f);
		viewport._size = Vector2((float)resolutionWidth, (float)resolutionHeight);

		// One unit per pixel, origin at the top left of the view, y up
		renderView->SetupCamera(Matrix4::OrthogonalProjection(0.0f, (float)resolutionWidth, -(float)resolutionHeight, 0.0f, -1024, 1024), Matrix4::Identity, viewport);

		// Each font drawn its own way: Pixel glyphs with a Nearest filter
		Sampler::CreateInfo nearestSamplerCreateInfo;
		nearestSamplerCreateInfo._filterMode = FilterMode::Nearest;
		Sampler::CreateInfo linearSamplerCreateInfo;
		linearSamplerCreateInfo._filterMode = FilterMode::Linear;
		const Sampler*  nearestSampler = RhiDevice::GetInstance()->GetSampler(nearestSamplerCreateInfo);
		const Sampler*  linearSampler = RhiDevice::GetInstance()->GetSampler(linearSamplerCreateInfo);
		const Material* material = BuiltinMaterialManager::GetInstance()->GetBuiltinMaterial(BuiltinMaterialManager::BuiltinMaterial::P2fT2f_Text_Bitmap);
		for (const TextMesh::Batch& batch : _mesh._batches)
		{
			// One per frame: the one of the previous frame may still be in use by the GPU
			MaterialInstance* materialInstance = MaterialInstance::Create(material);
			materialInstance->SetTexture("image", batch._glyphCache->GetPage(batch._pageIndex));
			materialInstance->SetSampler("imageSampler", batch._mode == GlyphCache::Mode::Pixel ? nearestSampler : linearSampler);
			materialInstance->SetVec4("ubo.color", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			frameResources.DeleteAfter(materialInstance);

			renderView->PushRenderCommand(DefaultAllocator::GetInstance().New<RenderCommandMesh>(batch._positions.Data(), batch._uvs.Data(), nullptr, batch._positions.Size(),
			                                                                                     batch._indices.Data(), batch._indices.Size(), Matrix4::Identity, materialInstance,
			                                                                                     0, 0, true));
		}

		ImDrawList* drawList = ImGui::GetWindowDrawList();
		drawList->AddImage(_renderTarget->GetColorTexture(), viewPosition, ImVec2(viewPosition.x + (float)resolutionWidth, viewPosition.y + (float)resolutionHeight));

		// The cells: their frame, the ones from a fallback in orange, the missing ones in red, the codepoints under them
		const ImU32 frameColor = IM_COL32(70, 70, 70, 255);
		const ImU32 fallbackColor = IM_COL32(230, 150, 40, 60);
		const ImU32 missingColor = IM_COL32(200, 60, 50, 90);
		const ImU32 labelColor = IM_COL32(140, 140, 140, 255);
		const ImU32 fallbackLabelColor = IM_COL32(255, 180, 80, 255);
		const ImU32 missingLabelColor = IM_COL32(255, 120, 110, 255);
		const ImVec2 mouse = ImGui::GetIO().MousePos;
		bool         hovered = false;
		ImVec2       hoveredCellMin;
		ImVec2       hoveredCellMax;
		for (uint32_t row = firstRow; row < lastRow; ++row)
		{
			for (uint32_t column = 0; column < columnCount; ++column)
			{
				const uint32_t cellIndex = row * columnCount + column;
				if (cellIndex >= _cells.Size())
				{
					break;
				}
				const Cell& cell = _cells[cellIndex];

				const ImVec2 cellMin(viewPosition.x + (float)column * CellSize, viewPosition.y + std::round((float)row * CellSize - scroll));
				const ImVec2 cellMax(cellMin.x + CellSize, cellMin.y + CellSize);
				if (cell._glyphId == 0)
				{
					drawList->AddRectFilled(cellMin, cellMax, missingColor);
				}
				else if (cell._fontIndex != 0)
				{
					drawList->AddRectFilled(cellMin, cellMax, fallbackColor);
				}
				drawList->AddRect(cellMin, ImVec2(cellMax.x + 1.0f, cellMax.y + 1.0f), frameColor);

				const String label = String::Format("%04X", (uint32_t)cell._codepoint);
				const ImVec2 labelSize = ImGui::CalcTextSize(label.CStr());
				drawList->AddText(ImVec2(cellMin.x + (CellSize - labelSize.x) * 0.5f, cellMax.y - LabelHeight + (LabelHeight - labelSize.y) * 0.5f),
				                  cell._glyphId == 0 ? missingLabelColor : (cell._fontIndex != 0 ? fallbackLabelColor : labelColor), label.CStr());

				if (ImGui::IsWindowHovered() && mouse.x >= cellMin.x && mouse.x < cellMax.x && mouse.y >= cellMin.y && mouse.y < cellMax.y)
				{
					hoveredCellMin = cellMin;
					hoveredCellMax = cellMax;
					hovered = true;
					if (cell._glyphId == 0)
					{
						ImGui::SetTooltip(_fontChain.GetFaces().Size() > 1 ? "U+%04X\nMissing from the font and its fallbacks" : "U+%04X\nMissing from the font",
						                  (uint32_t)cell._codepoint);
					}
					else if (cell._fontIndex != 0)
					{
						const FontFaceResource& fallback = *_fontChain.GetFaceResources()[cell._fontIndex];
						std::shared_ptr<Asset>  fallbackAsset = AssetDatabase::GetInstance()->Find(fallback.GetUid());
						const String            fallbackName = fallbackAsset != nullptr ? fallbackAsset->GetName() : fallback.GetUid().ToString();
						ImGui::SetTooltip("U+%04X\nMissing from the first face, drawn with %s\nGlyph %u\nAdvance %d units", (uint32_t)cell._codepoint, fallbackName.CStr(),
						                  cell._glyphId, fallback.GetFontFace().GetGlyphAdvance(cell._glyphId));
					}
					else
					{
						ImGui::SetTooltip("U+%04X\nGlyph %u\nAdvance %d units", (uint32_t)cell._codepoint, cell._glyphId, face.GetGlyphAdvance(cell._glyphId));
					}
				}
			}
		}

		// Over the frames of the cells after it: drawn last
		if (hovered)
		{
			drawList->AddRect(hoveredCellMin, ImVec2(hoveredCellMax.x + 1.0f, hoveredCellMax.y + 1.0f), IM_COL32(80, 160, 255, 255));
		}

		ImGui::EndChild();
	}
}

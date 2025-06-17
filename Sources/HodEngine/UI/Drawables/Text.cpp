#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Drawables/Text.hpp"
#include "HodEngine/UI/Node.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include <HodEngine/Game/Entity.hpp>

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Text, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Text::_font, "Font", &Text::SetFont);
		AddPropertyT(reflectionDescriptor, &Text::_value, "Value", &Text::SetValue);
		AddPropertyT(reflectionDescriptor, &Text::_alignment, "Alignment", &Text::SetAlignment);
	}

	void Text::PushToRenderQueue(renderer::RenderQueue& renderQueue)
	{
		if (_node.Get() && _font.Lock())
		{
			Vector2 size = _node->ComputeSize();
			Matrix4 worldMatrix = _node->ComputeCanvasMatrix();

			float atlasWidth = (float)_font.Lock()->GetTexture()->GetWidth();
			float atlasHeight = (float)_font.Lock()->GetTexture()->GetHeight();

			Vector<Vector2> positions;
			positions.reserve(_value.size() * 4);

			Vector<Vector2> uvs;
			uvs.reserve(_value.size() * 4);

			Vector<uint16_t> indices;
			indices.reserve(_value.size() * 6);

			Vector2 requiredSize;
			const char* str = _value.c_str();
			while (*str != '\0')
			{
				const renderer::FontResource::Kerning& kerning = _font.Lock()->GetKerning(*str);

				requiredSize.SetX(requiredSize.GetX() + (kerning._sizeX + 2) * 0.01f);
				if (requiredSize.GetY() < kerning._sizeY)
				{
					requiredSize.SetY((float)kerning._sizeY * 0.01f);
				}

				++str;
			}

			float offset = 0.0f;
			float baseLineOffset = 0.0f;
			switch (_alignment)
			{
			case Alignment::TopLeft:
				offset = -size.GetX() * 0.5f;
				baseLineOffset = size.GetY() * 0.5f - requiredSize.GetY();
				break;
			case Alignment::TopCenter:
				offset = -requiredSize.GetX() * 0.5f;
				baseLineOffset = size.GetY() * 0.5f - requiredSize.GetY();
				break;
			case Alignment::TopRight:
				offset = size.GetX() * 0.5f - requiredSize.GetX();
				baseLineOffset = size.GetY() * 0.5f - requiredSize.GetY();
				break;

			case Alignment::MiddleLeft:
				offset = -size.GetX() * 0.5f;
				baseLineOffset = -requiredSize.GetY() * 0.5f;
				break;
			case Alignment::MiddleCenter:
				offset = -requiredSize.GetX() * 0.5f;
				baseLineOffset = -requiredSize.GetY() * 0.5f;
				break;
			case Alignment::MiddleRight:
				offset = size.GetX() * 0.5f - requiredSize.GetX();
				baseLineOffset = -requiredSize.GetY() * 0.5f;
				break;

			case Alignment::BottomLeft:
				offset = -size.GetX() * 0.5f;
				baseLineOffset = -size.GetY() * 0.5f;
				break;
			case Alignment::BottomCenter:
				offset = -requiredSize.GetX() * 0.5f;
				baseLineOffset = -size.GetY() * 0.5f;
				break;
			case Alignment::BottomRight:
				offset = size.GetX() * 0.5f - requiredSize.GetX();
				baseLineOffset = -size.GetY() * 0.5f;
				break;
			}

			str = _value.c_str();
			while (*str != '\0')
			{
				const renderer::FontResource::Kerning& kerning = _font.Lock()->GetKerning(*str);

				uint16_t vertexCount = (uint16_t)positions.size();

				/*
				vertices.push_back(renderer::P2fT2f(offset + (float)kerning._sizeX * -0.5f * 0.01f, (float)kerning._sizeY * 0.5f * 0.01f, (float)kerning._offsetX / atlasWidth, (float)kerning._offsetY / atlasHeight));
				vertices.push_back(renderer::P2fT2f(offset + (float)kerning._sizeX * 0.5f * 0.01f, (float)kerning._sizeY * 0.5f * 0.01f, (float)(kerning._offsetX + kerning._sizeX) / atlasWidth, (float)kerning._offsetY / atlasHeight));
				vertices.push_back(renderer::P2fT2f(offset + (float)kerning._sizeX * 0.5f * 0.01f, (float)kerning._sizeY * -0.5f * 0.01f, (float)(kerning._offsetX + kerning._sizeX) / atlasWidth, (float)(kerning._offsetY + kerning._sizeY) / atlasHeight));
				vertices.push_back(renderer::P2fT2f(offset + (float)kerning._sizeX * -0.5f * 0.01f, (float)kerning._sizeY * -0.5f * 0.01f, (float)kerning._offsetX / atlasWidth, (float)(kerning._offsetY + kerning._sizeY) / atlasHeight));
				*/

				positions.emplace_back(offset, baseLineOffset + (float)kerning._baseline * 0.5f  * -0.01f + (float)kerning._sizeY * 0.5f * 0.01f);
				uvs.emplace_back((float)kerning._offsetX / atlasWidth, (float)kerning._offsetY / atlasHeight);

				positions.emplace_back(offset + (float)kerning._sizeX * 0.01f, baseLineOffset + (float)kerning._baseline * 0.5f  * -0.01f + (float)kerning._sizeY * 0.5f * 0.01f);
				uvs.emplace_back((float)(kerning._offsetX + kerning._sizeX) / atlasWidth, (float)kerning._offsetY / atlasHeight);

				positions.emplace_back(offset + (float)kerning._sizeX * 0.01f, baseLineOffset + (float)kerning._baseline * 0.5f  * -0.01f + (float)kerning._sizeY * -0.5f * 0.01f);
				uvs.emplace_back((float)(kerning._offsetX + kerning._sizeX) / atlasWidth, (float)(kerning._offsetY + kerning._sizeY) / atlasHeight);

				positions.emplace_back(offset, baseLineOffset + (float)kerning._baseline * 0.5f * -0.01f + (float)kerning._sizeY * -0.5f * 0.01f);
				uvs.emplace_back((float)kerning._offsetX / atlasWidth, (float)(kerning._offsetY + kerning._sizeY) / atlasHeight);

				indices.push_back(vertexCount);
				indices.push_back(vertexCount + 1);
				indices.push_back(vertexCount + 2);

				indices.push_back(vertexCount);
				indices.push_back(vertexCount + 2);
				indices.push_back(vertexCount + 3);

				offset += (kerning._sizeX + 2) * 0.01f;

				++str;
			}

			if (positions.empty() == false)
			{
				if (_materialInstance == nullptr)
				{
					const renderer::Material* material = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
					_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
				}

				_materialInstance->SetTexture("image", _font.Lock()->GetTexture());
				Vector4 vec4Color;
				vec4Color.SetX(_color.r);
				vec4Color.SetY(_color.g);
				vec4Color.SetZ(_color.b);
				vec4Color.SetW(_color.a);
				_materialInstance->SetVec4("UBO.color", vec4Color);

				renderQueue.PushRenderCommand(DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(positions.data(), uvs.data(), nullptr, (uint32_t)positions.size(), indices.data(), (uint32_t)indices.size(), worldMatrix, _materialInstance, 0, 0));
			}
		}
	}

	/// @brief 
	/// @param font 
	void Text::SetFont(const WeakResource<renderer::FontResource>& font)
	{
		_font = font;
	}

	/// @brief 
	/// @param value 
	void Text::SetValue(const String& value)
	{
		_value = value;
	}

	/// @brief 
	/// @param alignment 
	void Text::SetAlignment(Alignment alignment)
	{
		_alignment = alignment;
	}
}

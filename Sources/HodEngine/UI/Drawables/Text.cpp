#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Drawables/Text.hpp"
#include "HodEngine/UI/Node.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderView.hpp"
#include <HodEngine/Renderer/Font/Font.hpp>

#include <HodEngine/Game/Entity.hpp>
#include <HodEngine/Game/World.hpp>

namespace hod::ui
{
	DESCRIBE_REFLECTED_ENUM(Text::Alignment, reflectionDescriptor)
	{
		reflectionDescriptor.AddEnumValue(Text::Alignment::TopLeft, "TopLeft");
		reflectionDescriptor.AddEnumValue(Text::Alignment::TopCenter, "TopCenter");
		reflectionDescriptor.AddEnumValue(Text::Alignment::TopRight, "TopRight");
		reflectionDescriptor.AddEnumValue(Text::Alignment::MiddleLeft, "MiddleLeft");
		reflectionDescriptor.AddEnumValue(Text::Alignment::MiddleCenter, "MiddleCenter");
		reflectionDescriptor.AddEnumValue(Text::Alignment::MiddleRight, "MiddleRight");
		reflectionDescriptor.AddEnumValue(Text::Alignment::BottomLeft, "BottomLeft");
		reflectionDescriptor.AddEnumValue(Text::Alignment::BottomCenter, "BottomCenter");
		reflectionDescriptor.AddEnumValue(Text::Alignment::BottomRight, "BottomRight");
	}

	DESCRIBE_REFLECTED_CLASS(Text, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Text::_font, "Font", &Text::SetFont);
		AddPropertyT(reflectionDescriptor, &Text::_value, "Value", &Text::SetValue);
		AddPropertyT(reflectionDescriptor, &Text::_alignment, "Alignment", &Text::SetAlignment);
	}

	void Text::OnDestruct()
	{
		DefaultAllocator::GetInstance().Delete(_materialInstance);
		_materialInstance = nullptr;
	}

	void Text::PushRenderCommand(renderer::RenderView& renderView, renderer::RenderView::RenderQueueType renderQueueType)
	{
		if (_node.Get() && _font.Lock())
		{
			Vector2 size = _node->ComputeSize();
			Matrix4 worldMatrix = _node->ComputeWorldMatrix();

			renderer::Font* font = _font.Lock()->GetFont();

			Vector<Vector2> positions;
			positions.reserve(_value.Size() * 4);

			Vector<Vector2> uvs;
			uvs.reserve(_value.Size() * 4);

			Vector<uint16_t> indices;
			indices.reserve(_value.Size() * 6);

			Vector2 requiredSize = font->ComputeRequiredSize(_value);

			Vector2 lineTopLeftCorner;
			switch (_alignment)
			{
				case Alignment::TopLeft: lineTopLeftCorner = Vector2(-size.GetX() * 0.5f, size.GetY() * 0.5f); break;
				case Alignment::TopCenter: lineTopLeftCorner = Vector2(-requiredSize.GetX() * 0.5f, size.GetY() * 0.5f); break;
				case Alignment::TopRight: lineTopLeftCorner = Vector2(size.GetX() * 0.5f - requiredSize.GetX(), size.GetY() * 0.5f); break;

				case Alignment::MiddleLeft: lineTopLeftCorner = Vector2(-size.GetX() * 0.5f, requiredSize.GetY() * 0.5f); break;
				case Alignment::MiddleCenter: lineTopLeftCorner = Vector2(-requiredSize.GetX() * 0.5f, requiredSize.GetY() * 0.5f); break;
				case Alignment::MiddleRight: lineTopLeftCorner = Vector2(size.GetX() * 0.5f - requiredSize.GetX(), requiredSize.GetY() * 0.5f); break;

				case Alignment::BottomLeft: lineTopLeftCorner = Vector2(-size.GetX() * 0.5f, -size.GetY() * 0.5f + requiredSize.GetY()); break;
				case Alignment::BottomCenter: lineTopLeftCorner = Vector2(-requiredSize.GetX() * 0.5f, -size.GetY() * 0.5f + requiredSize.GetY()); break;
				case Alignment::BottomRight: lineTopLeftCorner = Vector2(size.GetX() * 0.5f - requiredSize.GetX(), -size.GetY() * 0.5f + requiredSize.GetY()); break;
			}

			/*
			GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner, worldMatrix.GetTranslation() + lineTopLeftCorner + Vector2(requiredSize.GetX(), 0.0f),
			Color::White); GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner + Vector2(0.0f, -requiredSize.GetY()), worldMatrix.GetTranslation() +
			lineTopLeftCorner + Vector2(requiredSize.GetX(), -requiredSize.GetY()), Color::White);

			GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner, worldMatrix.GetTranslation() + lineTopLeftCorner + Vector2(0.0f, -requiredSize.GetY()),
			Color::White); GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner + Vector2(requiredSize.GetX(), 0.0f), worldMatrix.GetTranslation() +
			lineTopLeftCorner + Vector2(requiredSize.GetX(), -requiredSize.GetY()), Color::White);
			*/

			Vector<renderer::Font::GlyphGeometry> glyphGeometries;
			font->BuildTextGeometry(_value, glyphGeometries);

			for (const renderer::Font::GlyphGeometry& glyphGeometry : glyphGeometries)
			{
				uint16_t vertexCount = (uint16_t)positions.size();

				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + Vector2(-glyphGeometry._posSize.GetX() * 0.5f, glyphGeometry._posSize.GetY() * 0.5f));
				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + Vector2(glyphGeometry._posSize.GetX() * 0.5f, glyphGeometry._posSize.GetY() * 0.5f));
				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + Vector2(-glyphGeometry._posSize.GetX() * 0.5f, -glyphGeometry._posSize.GetY() * 0.5f));
				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + Vector2(glyphGeometry._posSize.GetX() * 0.5f, -glyphGeometry._posSize.GetY() * 0.5f));

				uvs.push_back(glyphGeometry._uvPos);
				uvs.push_back(glyphGeometry._uvPos + Vector2(glyphGeometry._uvSize.GetX(), 0.0f));
				uvs.push_back(glyphGeometry._uvPos + Vector2(0.0f, glyphGeometry._uvSize.GetY()));
				uvs.push_back(glyphGeometry._uvPos + Vector2(glyphGeometry._uvSize.GetX(), glyphGeometry._uvSize.GetY()));

				indices.push_back(vertexCount + 1);
				indices.push_back(vertexCount + 0);
				indices.push_back(vertexCount + 2);

				indices.push_back(vertexCount + 1);
				indices.push_back(vertexCount + 2);
				indices.push_back(vertexCount + 3);
			}

			if (positions.empty() == false)
			{
				if (_materialInstance == nullptr)
				{
					const renderer::Material* material =
						renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
					_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
				}
				_materialInstance->SetTexture("image", font->GetTexture());
				Vector4 vec4Color;
				vec4Color.SetX(_color.r);
				vec4Color.SetY(_color.g);
				vec4Color.SetZ(_color.b);
				vec4Color.SetW(_color.a);
				_materialInstance->SetVec4("ubo.color", vec4Color);

				renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(positions.data(), uvs.data(), nullptr, (uint32_t)positions.size(),
				                                                                                              indices.data(), (uint32_t)indices.size(), worldMatrix,
				                                                                                              _materialInstance, 0, 0),
				                             renderQueueType);
			}
		}
	}

	/// @brief
	/// @param font
	void Text::SetFont(const WeakResource<renderer::FontResource>& font)
	{
		if (_font != font)
		{
			_font = font;
			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @param value
	void Text::SetValue(const String& value)
	{
		if (_value != value)
		{
			_value = value;
			_propertyChangedEvent.Emit();
		}
	}

	/// @brief
	/// @param alignment
	void Text::SetAlignment(Alignment alignment)
	{
		if (_alignment != alignment)
		{
			_alignment = alignment;
			_propertyChangedEvent.Emit();
		}
	}

	Vector2 Text::GetContentSize() const
	{
		Vector2 requiredSize;
		if (_font.Lock())
		{
			renderer::Font* font = _font.Lock()->GetFont();
			requiredSize = font->ComputeRequiredSize(_value);
		}
		return requiredSize;
	}

	Text::PropertyChangedEvent& Text::GetPropertyChangedEvent()
	{
		return _propertyChangedEvent;
	}
}

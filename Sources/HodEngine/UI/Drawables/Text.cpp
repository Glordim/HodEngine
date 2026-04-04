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
			math::Vector2 Size = _node->ComputeSize();
			math::Matrix4 worldMatrix = _node->ComputeWorldMatrix();

			renderer::Font* font = _font.Lock()->GetFont();

			Vector<math::Vector2> positions;
			positions.Reserve(_value.Size() * 4);

			Vector<math::Vector2> uvs;
			uvs.Reserve(_value.Size() * 4);

			Vector<uint16_t> indices;
			indices.Reserve(_value.Size() * 6);

			math::Vector2 requiredSize = font->ComputeRequiredSize(_value);

			math::Vector2 lineTopLeftCorner;
			switch (_alignment)
			{
				case Alignment::TopLeft: lineTopLeftCorner = math::Vector2(-Size.GetX() * 0.5f, Size.GetY() * 0.5f); break;
				case Alignment::TopCenter: lineTopLeftCorner = math::Vector2(-requiredSize.GetX() * 0.5f, Size.GetY() * 0.5f); break;
				case Alignment::TopRight: lineTopLeftCorner = math::Vector2(Size.GetX() * 0.5f - requiredSize.GetX(), Size.GetY() * 0.5f); break;

				case Alignment::MiddleLeft: lineTopLeftCorner = math::Vector2(-Size.GetX() * 0.5f, requiredSize.GetY() * 0.5f); break;
				case Alignment::MiddleCenter: lineTopLeftCorner = math::Vector2(-requiredSize.GetX() * 0.5f, requiredSize.GetY() * 0.5f); break;
				case Alignment::MiddleRight: lineTopLeftCorner = math::Vector2(Size.GetX() * 0.5f - requiredSize.GetX(), requiredSize.GetY() * 0.5f); break;

				case Alignment::BottomLeft: lineTopLeftCorner = math::Vector2(-Size.GetX() * 0.5f, -Size.GetY() * 0.5f + requiredSize.GetY()); break;
				case Alignment::BottomCenter: lineTopLeftCorner = math::Vector2(-requiredSize.GetX() * 0.5f, -Size.GetY() * 0.5f + requiredSize.GetY()); break;
				case Alignment::BottomRight: lineTopLeftCorner = math::Vector2(Size.GetX() * 0.5f - requiredSize.GetX(), -Size.GetY() * 0.5f + requiredSize.GetY()); break;
			}

			/*
			GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner, worldMatrix.GetTranslation() + lineTopLeftCorner + math::Vector2(requiredSize.GetX(), 0.0f),
			math::Color::White); GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner + math::Vector2(0.0f, -requiredSize.GetY()), worldMatrix.GetTranslation() +
			lineTopLeftCorner + math::Vector2(requiredSize.GetX(), -requiredSize.GetY()), math::Color::White);

			GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner, worldMatrix.GetTranslation() + lineTopLeftCorner + math::Vector2(0.0f, -requiredSize.GetY()),
			math::Color::White); GetWorld()->DrawDebugLine(worldMatrix.GetTranslation() + lineTopLeftCorner + math::Vector2(requiredSize.GetX(), 0.0f), worldMatrix.GetTranslation() +
			lineTopLeftCorner + math::Vector2(requiredSize.GetX(), -requiredSize.GetY()), math::Color::White);
			*/

			Vector<renderer::Font::GlyphGeometry> glyphGeometries;
			font->BuildTextGeometry(_value, glyphGeometries);

			for (const renderer::Font::GlyphGeometry& glyphGeometry : glyphGeometries)
			{
				uint16_t vertexCount = (uint16_t)positions.Size();

				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + math::Vector2(-glyphGeometry._posSize.GetX() * 0.5f, glyphGeometry._posSize.GetY() * 0.5f));
				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + math::Vector2(glyphGeometry._posSize.GetX() * 0.5f, glyphGeometry._posSize.GetY() * 0.5f));
				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + math::Vector2(-glyphGeometry._posSize.GetX() * 0.5f, -glyphGeometry._posSize.GetY() * 0.5f));
				positions.push_back(lineTopLeftCorner + glyphGeometry._posCenter + math::Vector2(glyphGeometry._posSize.GetX() * 0.5f, -glyphGeometry._posSize.GetY() * 0.5f));

				uvs.push_back(glyphGeometry._uvPos);
				uvs.push_back(glyphGeometry._uvPos + math::Vector2(glyphGeometry._uvSize.GetX(), 0.0f));
				uvs.push_back(glyphGeometry._uvPos + math::Vector2(0.0f, glyphGeometry._uvSize.GetY()));
				uvs.push_back(glyphGeometry._uvPos + math::Vector2(glyphGeometry._uvSize.GetX(), glyphGeometry._uvSize.GetY()));

				indices.push_back(vertexCount + 1);
				indices.push_back(vertexCount + 0);
				indices.push_back(vertexCount + 2);

				indices.push_back(vertexCount + 1);
				indices.push_back(vertexCount + 2);
				indices.push_back(vertexCount + 3);
			}

			if (positions.Empty() == false)
			{
				if (_materialInstance == nullptr)
				{
					const renderer::Material* material =
						renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
					_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
				}
				_materialInstance->SetTexture("image", font->GetTexture());
				math::Vector4 vec4Color;
				vec4Color.SetX(_color.r);
				vec4Color.SetY(_color.g);
				vec4Color.SetZ(_color.b);
				vec4Color.SetW(_color.a);
				_materialInstance->SetVec4("ubo.color", vec4Color);

				renderView.PushRenderCommand(DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(positions.Data(), uvs.Data(), nullptr, (uint32_t)positions.Size(),
				                                                                                              indices.Data(), (uint32_t)indices.Size(), worldMatrix,
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

	math::Vector2 Text::GetContentSize() const
	{
		math::Vector2 requiredSize;
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

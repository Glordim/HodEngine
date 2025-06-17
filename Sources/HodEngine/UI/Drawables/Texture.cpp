#include "HodEngine/UI/Pch.hpp"
#include "HodEngine/UI/Drawables/Texture.hpp"
#include "HodEngine/UI/Node.hpp"

#include "HodEngine/Renderer/MaterialManager.hpp"
#include "HodEngine/Renderer/Renderer.hpp"
#include "HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp"

#include <HodEngine/Game/Entity.hpp>

namespace hod::ui
{
	DESCRIBE_REFLECTED_CLASS(Texture, reflectionDescriptor)
	{
		AddPropertyT(reflectionDescriptor, &Texture::_texture, "Texture", &Texture::SetTexture);
	}

	void Texture::PushToRenderQueue(renderer::RenderQueue& renderQueue)
	{
		if (_node.Get())
		{
			Rect bb = GetBoundingBox();

			Vector2 size = _node->ComputeSize();
			Matrix4 worldMatrix = _node->ComputeCanvasMatrix();

			std::array<Vector2, 4> vertices = {
				Vector2(-0.5f * size.GetX(), 0.5f * size.GetY()),
				Vector2(0.5f * size.GetX(), 0.5f * size.GetY()),
				Vector2(0.5f * size.GetX(), -0.5f * size.GetY()),
				Vector2(-0.5f * size.GetX(), -0.5f * size.GetY()),
			};

			static std::array<Vector2, 4> uvs = {
				Vector2(0, 0),
				Vector2(1, 0),
				Vector2(1, 1),
				Vector2(0, 1),
			};

			static std::array<uint16_t, 3*2> indices = {
				0, 1, 2,
				0, 2, 3,
			};

			if (_materialInstance == nullptr)
			{
				const renderer::Material* material = renderer::MaterialManager::GetInstance()->GetBuiltinMaterial(renderer::MaterialManager::BuiltinMaterial::P2fT2f_Texture_Unlit_Color);
				_materialInstance = renderer::Renderer::GetInstance()->CreateMaterialInstance(material);
			}

			_materialInstance->SetTexture("image", _texture.Lock()->GetTexture());
			renderQueue.PushRenderCommand(DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(vertices.data(), uvs.data(), nullptr, (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), worldMatrix, _materialInstance, 0, 0));
		}
	}

	/// @brief 
	/// @param texture 
	void Texture::SetTexture(const WeakResource<renderer::TextureResource>& texture)
	{
		_texture = texture;
	}
}

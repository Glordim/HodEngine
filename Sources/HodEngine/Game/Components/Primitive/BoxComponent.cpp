#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Primitive/BoxComponent.hpp"
#include "HodEngine/Game/Components/Node2dComponent.hpp"
#include "HodEngine/Game/Entity.hpp"

#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Sprite.hpp>
#include <HodEngine/Renderer/SpriteAtlas.hpp>
#include <HodEngine/Renderer/P2fT2f.hpp>

#include <array>

namespace hod
{
	namespace game
	{
		// 0_____3
		// |     |
		// |_____|
		// 1     2

		static std::array<Vector2, 4> _positions = {
			Vector2(-0.5f, 0.5f),
			Vector2(-0.5f, -0.5f),
			Vector2(0.5f, -0.5f),
			Vector2(0.5f, 0.5f),
		};

		// todo remove ?
		static std::array<Vector2, 4> _uvs = {
			Vector2(0, 0),
			Vector2(0, 0),
			Vector2(0, 0),
			Vector2(0, 0),
		};

		static std::array<uint16_t, 6> _indices = {
			0, 1, 3,
			3, 1, 2,
		};

		DESCRIBE_REFLECTED_CLASS(BoxComponent, reflectionDescriptor)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		BoxComponent::~BoxComponent()
		{

		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		void BoxComponent::PushToRenderQueue(renderer::RenderQueue& renderQueue)
		{
			Entity* entity = GetOwner();
			if (entity != nullptr)
			{
				Node2dComponent* node2dComponent = entity->GetComponent<Node2dComponent>();
				if (node2dComponent != nullptr)
				{
					renderQueue.PushRenderCommand(DefaultAllocator::GetInstance().New<renderer::RenderCommandMesh>(_positions.data(), _uvs.data(), nullptr, 4, _indices.data(), (uint32_t)_indices.size(), node2dComponent->GetWorldMatrix(), nullptr, node2dComponent->GetZOrder().GetValue(), (uint32_t)entity->GetInstanceId()));
				}
			}
		}
	}
}

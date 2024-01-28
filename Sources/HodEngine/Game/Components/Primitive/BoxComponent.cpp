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

		static std::array<renderer::P2fT2f, 4> _vertices = {
			renderer::P2fT2f(-0.5f, 0.5f, 0, 0),
			renderer::P2fT2f(-0.5f, -0.5f, 0, 0),
			renderer::P2fT2f(0.5f, -0.5f, 0, 0),
			renderer::P2fT2f(0.5f, 0.5f, 0, 0),
		};

		static std::array<uint16_t, 3*2> _indices = {
			0, 1, 3,
			3, 1, 2,
		};

		DESCRIBE_REFLECTED_CLASS(BoxComponent, PrimitiveComponent)
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
			std::shared_ptr<Entity> entity = GetEntity().lock();
			if (entity != nullptr)
			{
				std::shared_ptr<Node2dComponent> node2dComponent = entity->GetComponent<Node2dComponent>().lock();
				if (node2dComponent != nullptr)
				{
					renderQueue.PushRenderCommand(new renderer::RenderCommandMesh(_vertices.data(), _vertices.size(), sizeof(renderer::P2fT2f), _indices.data(), _indices.size(), node2dComponent->GetWorldMatrix(), nullptr));
				}
			}
		}
	}
}

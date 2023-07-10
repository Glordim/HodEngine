#include "HodEngine/Game/Components/Primitive/BoxComponent.h"
#include "HodEngine/Game/Components/Node2dComponent.h"
#include "HodEngine/Game/Entity.h"

#include <HodEngine/Renderer/RHI/Material.h>
#include <HodEngine/Renderer/RenderQueue.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.h>
#include <HodEngine/Renderer/RHI/MaterialInstance.h>
#include <HodEngine/Renderer/Sprite.h>
#include <HodEngine/Renderer/SpriteAtlas.h>
#include <HodEngine/Renderer/P2fT2f.h>

#include "HodEngine/Game/Actor.h"

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
			//ADD_PROPERTY(SpriteComponent, _sprite);
			//ADD_PROPERTY(SpriteComponent, _materialInstance);
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

#include "HodEngine/Game/Components/Primitive/PrimitiveComponent.hpp"

#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RenderQueue.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Sprite.hpp>
#include <HodEngine/Renderer/SpriteAtlas.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(PrimitiveComponent, RendererComponent)
		{
			//ADD_PROPERTY(SpriteComponent, _sprite);
			//ADD_PROPERTY(SpriteComponent, _materialInstance);
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PrimitiveComponent::~PrimitiveComponent()
		{

		}
	}
}

#include "HodEngine/Game/Components/Primitive/PrimitiveComponent.h"

#include <HodEngine/Renderer/RHI/Material.h>
#include <HodEngine/Renderer/RenderQueue.h>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.h>
#include <HodEngine/Renderer/RHI/MaterialInstance.h>
#include <HodEngine/Renderer/Sprite.h>
#include <HodEngine/Renderer/SpriteAtlas.h>

#include "HodEngine/Game/Actor.h"

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

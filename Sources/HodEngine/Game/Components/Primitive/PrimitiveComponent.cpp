#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/Components/Primitive/PrimitiveComponent.hpp"

#include <HodEngine/Renderer/RHI/Material.hpp>
#include <HodEngine/Renderer/RenderCommand/RenderCommandMesh.hpp>
#include <HodEngine/Renderer/RHI/MaterialInstance.hpp>
#include <HodEngine/Renderer/Sprite.hpp>
#include <HodEngine/Renderer/SpriteAtlas.hpp>

namespace hod
{
	namespace game
	{
		DESCRIBE_REFLECTED_CLASS(PrimitiveComponent, reflectionDescriptor)
		{
		}

		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		PrimitiveComponent::~PrimitiveComponent()
		{

		}
	}
}

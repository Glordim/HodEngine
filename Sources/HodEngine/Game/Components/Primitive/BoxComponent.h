#pragma once

#include "HodEngine/Game/Components/Primitive/PrimitiveComponent.h"

namespace hod
{
	namespace renderer
	{
		class Sprite;
		class MaterialInstance;
	}

	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class BoxComponent : public PrimitiveComponent
		{
			REFLECTED_CLASS(BoxComponent, PrimitiveComponent)

		public:

							BoxComponent() = default;
							BoxComponent(const BoxComponent&) = delete;
							BoxComponent(BoxComponent&&) = delete;
							~BoxComponent() override;

			BoxComponent&	operator=(const BoxComponent&) = delete;
			BoxComponent&	operator=(BoxComponent&&) = delete;

		public:

			void			PushToRenderQueue(renderer::RenderQueue& renderQueue) override;
		};
	}
}

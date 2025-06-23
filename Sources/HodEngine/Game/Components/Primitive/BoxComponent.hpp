#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Primitive/PrimitiveComponent.hpp"

namespace hod
{
	namespace renderer
	{
		class MaterialInstance;
	}

	namespace game
	{
		//-----------------------------------------------------------------------------
		//! @brief		
		//-----------------------------------------------------------------------------
		class HOD_GAME_API BoxComponent : public PrimitiveComponent
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

			void			PushRenderCommand(renderer::RenderView& renderView) override;
		};
	}
}

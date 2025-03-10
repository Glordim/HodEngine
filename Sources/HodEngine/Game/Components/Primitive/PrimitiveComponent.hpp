#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"

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
		class HOD_GAME_API PrimitiveComponent : public RendererComponent
		{
			REFLECTED_CLASS(PrimitiveComponent, RendererComponent)

		public:

											PrimitiveComponent() = default;
											PrimitiveComponent(const PrimitiveComponent&) = delete;
											PrimitiveComponent(PrimitiveComponent&&) = delete;
											~PrimitiveComponent() override;

			PrimitiveComponent&				operator=(const PrimitiveComponent&) = delete;
			PrimitiveComponent&				operator=(PrimitiveComponent&&) = delete;
		};
	}
}

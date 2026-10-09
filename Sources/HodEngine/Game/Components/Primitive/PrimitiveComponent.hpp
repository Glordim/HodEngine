#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/RendererComponent.hpp"

namespace hod::inline renderer { class MaterialInstance; }

namespace hod::inline game
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

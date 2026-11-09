#pragma once
#include "HodEngine/RendererComponents/Export.hpp"

#include "HodEngine/RendererComponents/Components/RendererComponent.hpp"

namespace hod::inline renderer { class MaterialInstance; }

namespace hod::inline renderer
{
	//-----------------------------------------------------------------------------
	//! @brief		
	//-----------------------------------------------------------------------------
	class HOD_RENDERER_COMPONENTS_API PrimitiveComponent : public RendererComponent
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

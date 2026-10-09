#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Components/Primitive/PrimitiveComponent.hpp"
#include "HodEngine/GameSystems/Resource/WeakResource.hpp"
#include "HodEngine/Renderer/Resource/MaterialInstanceResource.hpp"

namespace hod::inline renderer { class MaterialInstance; }

namespace hod::inline game
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

		void			OnConstruct() override;

		void			PushRenderCommand(RenderView& renderView) override;
		Rect			GetBoundingBox() const override;

		MaterialInstance*		GetMaterialInstance() const;

		void							SetMaterialInstanceResource(const WeakResource<MaterialInstanceResource>& materialInstance);

	private:

		WeakResource<MaterialInstanceResource>	_material;

		MaterialInstance*							_materialInstance = nullptr;
	};
}

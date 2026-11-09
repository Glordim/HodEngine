#include "HodEngine/RendererEditor/Pch.hpp"
#include "HodEngine/RendererEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

#include "HodEngine/RendererEditor/FontEditor/FontAssetType.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceAssetType.hpp"
#include "HodEngine/RendererEditor/FontFaceEditor/FontFaceImporter.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialAssetType.hpp"
#include "HodEngine/RendererEditor/MaterialEditor/MaterialImporter.hpp"
#include "HodEngine/RendererEditor/MaterialInstanceEditor/MaterialInstanceAssetType.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureAssetType.hpp"
#include "HodEngine/RendererEditor/TextureEditor/TextureImporter.hpp"

#include "HodEngine/RendererEditor/CustomComponentDrawer/CameraCustomComponentDrawer.hpp"
#include "HodEngine/RendererEditor/CustomComponentDrawer/TextureRendererCustomComponentDrawer.hpp"

#include <HodEngine/GameEditor/EntityBasedTabEditor/ViewportWorldRenderer.hpp>
#include <HodEngine/RendererComponents/Components/CameraComponent.hpp>
#include <HodEngine/RendererComponents/Components/TextureRendererComponent.hpp>
#include <HodEngine/RendererComponents/RendererWorldSystem.hpp>

#include <HodEngine/Game/World.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	/// @brief The render of the worlds in the viewports (see ViewportWorldRenderer)
	void DrawWorldInViewport(RenderView& renderView, game::World& world)
	{
		if (RendererWorldSystem* rendererWorldSystem = RendererWorldSystem::Get(world))
		{
			rendererWorldSystem->Draw(renderView);
		}
	}
}

HOD_STARTUP_EDITOR_MODULE(RendererEditor, registration)
{
	registration.RegisterCustomComponentDrawer<CameraComponent, CameraCustomComponentDrawer>();
	registration.RegisterCustomComponentDrawer<TextureRendererComponent, TextureRendererCustomComponentDrawer>();

	ViewportWorldRenderer::Set(&DrawWorldInViewport);

	// The importers after the types they import into, only if registered: an importer producing several types would need them all
	if (registration.RegisterAssetType<TextureAssetType>())
	{
		registration.RegisterImporter<TextureImporter>("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
	}

	if (registration.RegisterAssetType<FontFaceAssetType>())
	{
		registration.RegisterImporter<FontFaceImporter>("ttf", "otf");
	}

	if (registration.RegisterAssetType<MaterialAssetType>())
	{
		registration.RegisterImporter<MaterialImporter>("slang");
	}

	registration.RegisterAssetType<FontAssetType>();
	registration.RegisterAssetType<MaterialInstanceAssetType>();
	return 0;
}

HOD_SHUTDOWN_EDITOR_MODULE(RendererEditor)
{
	ViewportWorldRenderer::Set(nullptr);
	return 0;
}

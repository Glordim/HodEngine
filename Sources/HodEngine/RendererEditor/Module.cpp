#include "HodEngine/RendererEditor/Pch.hpp"
#include <HodEngine/Core/Module.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/Editor/EditorRegistrar.hpp"

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

#include <HodEngine/GameEditor/GameEditorRegistrar.hpp>
#include <HodEngine/RendererComponents/Components/CameraComponent.hpp>
#include <HodEngine/RendererComponents/Components/TextureRendererComponent.hpp>
#include <HodEngine/RendererComponents/RendererWorldSystem.hpp>

#include <HodEngine/Game/World.hpp>

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline renderer
{
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

		/// @brief The game's camera of the worlds in the viewports, when playing (see ViewportWorldRenderer)
		void SetupPlayCameraInViewport(RenderView& renderView, game::World& world)
		{
			if (RendererWorldSystem* rendererWorldSystem = RendererWorldSystem::Get(world))
			{
				rendererWorldSystem->SetupPlayCamera(renderView);
			}
		}
	}

	HOD_STARTUP_MODULE(RendererEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterCustomComponentDrawer<CameraComponent, CameraCustomComponentDrawer>();
		editorRegistrar.RegisterCustomComponentDrawer<TextureRendererComponent, TextureRendererCustomComponentDrawer>();

		GameEditorRegistrar gameEditorRegistrar(registration);
		gameEditorRegistrar.SetViewportWorldRenderer(&DrawWorldInViewport, &SetupPlayCameraInViewport);

		// The importers after the types they import into, only if registered: an importer producing several types would need them all
		if (editorRegistrar.RegisterAssetType<TextureAssetType>())
		{
			editorRegistrar.RegisterImporter<TextureImporter>("png", "tga", "jpg", "bmp", "psd", "gif", "hdr", "pic");
		}

		if (editorRegistrar.RegisterAssetType<FontFaceAssetType>())
		{
			editorRegistrar.RegisterImporter<FontFaceImporter>("ttf", "otf");
		}

		if (editorRegistrar.RegisterAssetType<MaterialAssetType>())
		{
			editorRegistrar.RegisterImporter<MaterialImporter>("slang");
		}

		editorRegistrar.RegisterAssetType<FontAssetType>();
		editorRegistrar.RegisterAssetType<MaterialInstanceAssetType>();
		return true;
	}
}

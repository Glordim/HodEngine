#include "HodEngine/AudioEditor/Pch.hpp"
#include <HodEngine/Editor/EditorModule.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/AudioEditor/AudioAssetType.hpp"
#include "HodEngine/AudioEditor/AudioImporter.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline audio
{
	HOD_STARTUP_EDITOR_MODULE(AudioEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		if (editorRegistrar.RegisterAssetType<AudioAssetType>())
		{
			editorRegistrar.RegisterImporter<AudioImporter>("wav");
		}
		return true;
	}
}

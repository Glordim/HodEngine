#include "HodEngine/AudioEditor/Pch.hpp"
#include "HodEngine/AudioEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/AudioEditor/AudioAssetType.hpp"
#include "HodEngine/AudioEditor/AudioImporter.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

HOD_STARTUP_EDITOR_MODULE(AudioEditor, registration)
{
	hod::editor::EditorRegistrar editorRegistrar(registration);
	if (editorRegistrar.RegisterAssetType<AudioAssetType>())
	{
		editorRegistrar.RegisterImporter<AudioImporter>("wav");
	}
	return 0;
}

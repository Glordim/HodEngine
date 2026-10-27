#include "HodEngine/AudioEditor/Pch.hpp"
#include "HodEngine/AudioEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/AudioEditor/AudioCooker.hpp"
#include "HodEngine/AudioEditor/AudioEditorTab.hpp"
#include "HodEngine/AudioEditor/AudioImporter.hpp"

#include "HodEngine/Audio/AudioResource.hpp"

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

namespace
{
	EditorModuleRegistration registration;
}

HOD_STARTUP_MODULE(AudioEditor)
{
	registration.RegisterImporter<AudioImporter>("wav");
	registration.RegisterCooker<AudioCooker>("Audio");
	registration.RegisterResource<AudioResource>("Audio");
	registration.RegisterEditorTab<AudioEditorTab>("Audio");
	return 0;
}

HOD_SHUTDOWN_MODULE(AudioEditor)
{
	registration.UnregisterAll();
	return 0;
}

#include "HodEngine/VersionControlEditor/Pch.hpp"
#include "HodEngine/VersionControlEditor/Module.hpp"
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/VersionControlEditor/Git/GitProvider.hpp"

#include "HodEngine/Editor/EditorModuleRegistration.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

using namespace hod;
using namespace hod::editor;

// The version control systems offered to the user (see VersionControl::SetProviderChoice), "Auto" trying them in this order
HOD_STARTUP_EDITOR_MODULE(VersionControlEditor, registration)
{
	registration.RegisterVersionControlProvider<GitProvider>();
	return 0;
}

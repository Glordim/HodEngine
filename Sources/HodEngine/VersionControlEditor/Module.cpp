#include "HodEngine/VersionControlEditor/Pch.hpp"
#include <HodEngine/Editor/EditorModule.hpp>
#include <HodEngine/Core/Memory/MemoryOperator.hpp>

#include "HodEngine/VersionControlEditor/Git/GitProvider.hpp"

#include "HodEngine/Editor/EditorRegistrar.hpp"

REDIRECT_NEW_DELETE_OPERATOR_TO_MEMORY_MANAGER

namespace hod::inline editor
{
	// The version control systems offered to the user (see VersionControl::SetProviderChoice), "Auto" trying them in this order
	HOD_STARTUP_EDITOR_MODULE(VersionControlEditor, registration)
	{
		EditorRegistrar editorRegistrar(registration);
		editorRegistrar.RegisterVersionControlProvider<GitProvider>();
		return true;
	}
}

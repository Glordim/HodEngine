#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneResource.hpp"

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(SceneResource, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
	}

	/// @brief 
	SceneResource::~SceneResource()
	{
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool SceneResource::Initialize(const ResourceContainer& /*resourceContainer*/)
	{
		/*
		_document.GetRootNode().Copy(documentNode);
		return true;
		*/
		return false;
	}

	/// @brief 
	/// @return 
	Scene* SceneResource::CreateScene()
	{
		Scene* scene = DefaultAllocator::GetInstance().New<Scene>();
		if (scene->DeserializeFromDocument(_document.GetRootNode()) == false)
		{
			DefaultAllocator::GetInstance().Delete(scene);
			return nullptr;
		}
		return scene;
	}
}

#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneResource.hpp"

#include "HodEngine/Core/Document/DocumentReaderJson.hpp"
#include <HodEngine/GameSystems/Resource/ResourceContainer.hpp>

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
	bool SceneResource::Initialize(const ResourceContainer& resourceContainer)
	{
		const ResourceContainer::DataBlockInfo* entitiesDataBlock = resourceContainer.FindDataBlock("Entities");
		if (entitiesDataBlock == nullptr)
		{
			return false;
		}

		DocumentReaderJson documentReader;
		if (documentReader.Read(_document, *entitiesDataBlock->_stream) == false)
		{
			return false;
		}
		
		return true;
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

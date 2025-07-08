#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneResource.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(SceneResource, reflectionDescriptor)
	{
	}

	/// @brief 
	SceneResource::~SceneResource()
	{
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool SceneResource::Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas)
	{
		_document.GetRootNode().Copy(documentNode);
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

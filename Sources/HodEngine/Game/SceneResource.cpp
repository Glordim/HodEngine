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
		DefaultAllocator::GetInstance().Delete(_scene);
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool SceneResource::Initialize(const Document::Node& documentNode, FileSystem::Handle& fileHandle)
	{
		_scene = DefaultAllocator::GetInstance().New<Scene>();
		return _scene->DeserializeFromDocument(documentNode);
	}

	/// @brief 
	/// @return 
	Scene& SceneResource::GetScene()
	{
		return *_scene;
	}
}

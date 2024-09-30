#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Game/SceneResource.hpp"

namespace hod::game
{
	DESCRIBE_REFLECTED_CLASS(SceneResource, Resource)
	{
	}

	/// @brief 
	/// @param documentNode 
	/// @param stream 
	/// @return 
	bool SceneResource::Initialize(const Document::Node& documentNode, std::istream& stream)
	{
		_scene = new Scene();
		return _scene->DeserializeFromDocument(documentNode);
	}

	/// @brief 
	void SceneResource::Destroy()
	{
		delete _scene;
		_scene = nullptr;	
	}

	/// @brief 
	/// @return 
	Scene& SceneResource::GetScene()
	{
		return *_scene;
	}
}

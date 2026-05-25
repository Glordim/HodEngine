#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/UI2/Scene.hpp"
#include "HodEngine/UI2/Node.hpp"

namespace hod::inline ui2
{
	Scene::Scene()
	{
		_rootNode = DefaultAllocator::GetInstance().New<Node>();
		_rootNode->SetDeltaSize(_designResolution);
	}

	Scene::~Scene()
	{
		DefaultAllocator::GetInstance().Delete(_rootNode);
	}

	void Scene::AddChild(Node* node)
	{
		_rootNode->AddChild(node);
	}
	
	void Scene::RemoveChild(Node* node)
	{
		_rootNode->RemoveChild(node);
	}
}

#include "HodEngine/Game/Pch.hpp"
#include "HodEngine/Core/Memory/DefaultAllocator.hpp"
#include "HodEngine/Game/Components/UIComponent.hpp"
#include <HodEngine/UI2/Scene.hpp>
#include <HodEngine/UI2/SceneManager.hpp>

#include <HodEngine/UI2/Node.hpp>

namespace hod::inline game
{
	DESCRIBE_REFLECTED_CLASS(UIComponent, reflectionDescriptor)
	{
		(void)reflectionDescriptor;
		//AddPropertyT(reflectionDescriptor, &UIComponent::_scene, "Scene");
	}

	UIComponent::UIComponent()
	{

	}

	UIComponent::~UIComponent()
	{

	}

	void UIComponent::OnConstruct()
	{
		_scene = DefaultAllocator::GetInstance().New<ui2::Scene>();

		_scene->AddChild(DefaultAllocator::GetInstance().New<ui2::Node>());

		SceneManager::GetInstance()->AddScene(_scene);
	}

	void UIComponent::OnDestruct()
	{
		SceneManager::GetInstance()->RemoveScene(_scene);
		DefaultAllocator::GetInstance().Delete(_scene);
		_scene = nullptr;
	}
}

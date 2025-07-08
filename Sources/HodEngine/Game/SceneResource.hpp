#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Resource/Resource.hpp>
#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API SceneResource : public Resource
	{
		REFLECTED_CLASS(SceneResource, Resource)

	public:
						SceneResource() = default;
						SceneResource(const SceneResource&) = delete;
						SceneResource(SceneResource&&) = delete;
						~SceneResource() override;

		SceneResource&	operator=(const SceneResource&) = delete;
		SceneResource&	operator=(SceneResource&&) = delete;

	public:

		bool			Initialize(const Document::Node& documentNode, const Vector<Resource::Data>& datas) override;

		Scene*			CreateScene();

	private:

		Document		_document;
	};
}

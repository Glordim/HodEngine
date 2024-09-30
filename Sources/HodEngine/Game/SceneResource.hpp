#pragma once
#include "HodEngine/Game/Export.hpp"

#include <HodEngine/Core/Resource.hpp>
#include "HodEngine/Game/Scene.hpp"

namespace hod::game
{
	/// @brief 
	class HOD_GAME_API SceneResource : public Resource
	{
		REFLECTED_CLASS(SceneResource, Resource, HOD_GAME_API)

	public:
						SceneResource() = default;
						SceneResource(const SceneResource&) = delete;
						SceneResource(SceneResource&&) = delete;
						~SceneResource() override = default;

		SceneResource&	operator=(const SceneResource&) = delete;
		SceneResource&	operator=(SceneResource&&) = delete;

	public:

		bool			Initialize(const Document::Node& documentNode, std::istream& stream) override;
		void			Destroy() override;

		Scene&			GetScene();

	private:

		Scene*			_scene = nullptr;
	};
}

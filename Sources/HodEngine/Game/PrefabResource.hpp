#pragma once

#include <HodEngine/Core/Resource.hpp>
#include "HodEngine/Game/Prefab.hpp"

namespace hod::game
{
	/// @brief 
	class PrefabResource : public Resource
	{
		REFLECTED_CLASS(PrefabResource, Resource)

	public:
						PrefabResource() = default;
						PrefabResource(const PrefabResource&) = delete;
						PrefabResource(PrefabResource&&) = delete;
						~PrefabResource() override = default;

		PrefabResource&	operator=(const PrefabResource&) = delete;
		PrefabResource&	operator=(PrefabResource&&) = delete;

	public:

		bool			Initialize(const Document::Node& documentNode, std::istream& stream) override;
		void			Destroy() override;

		Prefab&			GetPrefab();

	private:

		Prefab*			_prefab = nullptr;
	};
}

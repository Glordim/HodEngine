#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/Core/Document/Document.hpp>

namespace hod::inline ui2
{
	class Canvas;

	/// @brief Runtime resource wrapping a cooked ui2::Canvas (a tree of ui2::Node), the UI2
	/// counterpart of Game::PrefabResource.
	class HOD_UI2_API UIPrefabResource : public Resource
	{
		REFLECTED_CLASS(UIPrefabResource, Resource)

	public:
							UIPrefabResource() = default;
							UIPrefabResource(const UIPrefabResource&) = delete;
							UIPrefabResource(UIPrefabResource&&) = delete;
							~UIPrefabResource() override;

		UIPrefabResource&	operator=(const UIPrefabResource&) = delete;
		UIPrefabResource&	operator=(UIPrefabResource&&) = delete;

	public:

		bool				Initialize(const ResourceContainer& resourceContainer) override;

		Canvas&				GetCanvas();
		const Document&		GetDocument() const;

	private:

		Canvas*				_canvas = nullptr;
		Document			_document;
	};
}

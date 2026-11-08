#pragma once
#include "HodEngine/UI/Export.hpp"

#include <HodEngine/GameSystems/Resource/Resource.hpp>
#include <HodEngine/Core/Document/Document.hpp>

namespace hod::inline ui
{
	class Canvas;

	/// @brief Runtime resource wrapping a cooked ui::Canvas (a tree of ui::Node), the UI
	/// counterpart of Game::PrefabResource.
	class HOD_UI_API UIPrefabResource : public Resource
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

#pragma once
#include "HodEngine/Game/Export.hpp"

#include "HodEngine/Game/Component.hpp"

#include <HodEngine/Core/Document/Document.hpp>

namespace hod::inline game
{
	/// @brief Stands for a component whose type isn't registered (its module is disabled, or gone): created in its place when a scene
	/// or a prefab is loaded, it keeps its data as they were and writes them back when saved, so nothing is lost. Does nothing at runtime.
	/// Not in the ComponentFactory: can't be added by the users.
	class HOD_GAME_API MissingComponent : public Component
	{
		REFLECTED_CLASS(MissingComponent, Component)

	public:
		MissingComponent() = default;
		MissingComponent(const MissingComponent&) = delete;
		MissingComponent(MissingComponent&&) = delete;
		~MissingComponent() override = default;

		MissingComponent& operator=(const MissingComponent&) = delete;
		MissingComponent& operator=(MissingComponent&&) = delete;

	public:
		/// @param componentNode The component as saved, its "MetaType" included
		void SetData(const DocumentNode& componentNode);

		/// @brief The component as saved, its "MetaType" included (see SetData)
		const DocumentNode& GetData() const;

		/// @brief The type of the missing component (see ReflectionDescriptor::GetType)
		uint64_t GetMissingType() const;

	private:
		Document _data;
	};
}

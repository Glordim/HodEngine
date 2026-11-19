#pragma once
#include "HodEngine/GameEditor/Export.hpp"
#include <HodEngine/Editor/PropertyDrawer/PropertyDrawer.hpp>

namespace hod::inline game
{
	class WeakComponentBase;
	class World;
}

namespace hod::inline editor
{
	/// @brief A reference to a component (WeakComponent<T>): the entity whose component it references, picked among the ones of the world of the
	/// Scope having one. Only drawn within a Scope, which says where the entities are
	class HOD_GAME_EDITOR_API WeakComponentDrawer : public PropertyDrawer<WeakComponentBase>
	{
	public:
		/// @brief While alive, the references drawn are picked among the entities of world
		class HOD_GAME_EDITOR_API Scope
		{
		public:
			explicit Scope(World& world);
			Scope(const Scope&) = delete;
			Scope(Scope&&) = delete;
			~Scope();

			Scope& operator=(const Scope&) = delete;
			Scope& operator=(Scope&&) = delete;

		private:
			World* _previousWorld = nullptr;
		};

	protected:
		bool OnDraw(const PropertyValues<WeakComponentBase>& values) override;
	};
}

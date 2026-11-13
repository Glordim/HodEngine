#pragma once
#include "HodEngine/UI/Export.hpp"

#include "HodEngine/UI/InteractionFactory.hpp"
#include "HodEngine/UI/LayoutFactory.hpp"
#include "HodEngine/UI/LayoutParamsFactory.hpp"
#include "HodEngine/UI/NodeFactory.hpp"
#include "HodEngine/UI/ViewFactory.hpp"

#include <HodEngine/Core/ModuleRegistration.hpp>

namespace hod::inline ui
{
	/// @brief The registrar of UI (see ModuleRegistration): nodes, layouts, layout params, interactions, views (UI's builtin ones, a game's),
	/// unregistered with the rest of what the module registered. In a runtime module's startup (see HOD_STARTUP_MODULE). Each returns false
	/// (with an error) if the type is registered already (by another module): the first one stays, nothing is undone for this one
	class UIRegistrar
	{
	public:
		explicit UIRegistrar(ModuleRegistration& registration)
		: _registration(registration)
		{
		}

	public:
		/// @brief A Node type, offered by the UI prefab editor
		template<typename _Node_>
		bool RegisterNode();

		/// @brief A Layout type (how a node places its children)
		template<typename _Layout_>
		bool RegisterLayout();

		/// @brief A LayoutParams type (how a node is placed by its parent's layout)
		template<typename _LayoutParams_>
		bool RegisterLayoutParams();

		/// @brief An Interaction type (Button, Slider...: a node's behavior under the pointer)
		template<typename _Interaction_>
		bool RegisterInteraction();

		/// @brief A View type (see View), set on the root node of UI prefabs
		template<typename _View_>
		bool RegisterView();

	private:
		/// @brief In _Factory_ (a singleton, may be destroyed before the unregister: nothing to do then)
		template<typename _Factory_, typename _Type_>
		bool Register();

	private:
		ModuleRegistration& _registration;
	};

	template<typename _Factory_, typename _Type_>
	bool UIRegistrar::Register()
	{
		if (_Factory_::GetInstance()->template Register<_Type_>() == false)
		{
			return false;
		}
		_registration.AddUnregister(
			[]()
			{
				if (_Factory_* factory = _Factory_::GetInstance())
				{
					factory->template Unregister<_Type_>();
				}
			});
		return true;
	}

	template<typename _Node_>
	bool UIRegistrar::RegisterNode()
	{
		return Register<NodeFactory, _Node_>();
	}

	template<typename _Layout_>
	bool UIRegistrar::RegisterLayout()
	{
		return Register<LayoutFactory, _Layout_>();
	}

	template<typename _LayoutParams_>
	bool UIRegistrar::RegisterLayoutParams()
	{
		return Register<LayoutParamsFactory, _LayoutParams_>();
	}

	template<typename _Interaction_>
	bool UIRegistrar::RegisterInteraction()
	{
		return Register<InteractionFactory, _Interaction_>();
	}

	template<typename _View_>
	bool UIRegistrar::RegisterView()
	{
		return Register<ViewFactory, _View_>();
	}
}

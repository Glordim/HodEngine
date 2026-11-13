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
	/// unregistered with the rest of what the module registered. In a runtime library's startup (see HOD_STARTUP_RUNTIME_MODULE)
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
		void RegisterNode();

		/// @brief A Layout type (how a node places its children)
		template<typename _Layout_>
		void RegisterLayout();

		/// @brief A LayoutParams type (how a node is placed by its parent's layout)
		template<typename _LayoutParams_>
		void RegisterLayoutParams();

		/// @brief An Interaction type (Button, Slider...: a node's behavior under the pointer)
		template<typename _Interaction_>
		void RegisterInteraction();

		/// @brief A View type (see View), set on the root node of UI prefabs
		template<typename _View_>
		void RegisterView();

	private:
		/// @brief In _Factory_ (a singleton, may be destroyed before the unregister: nothing to do then)
		template<typename _Factory_, typename _Type_>
		void Register();

	private:
		ModuleRegistration& _registration;
	};

	template<typename _Factory_, typename _Type_>
	void UIRegistrar::Register()
	{
		_Factory_::GetInstance()->template Register<_Type_>();
		_registration.AddUnregister(
			[]()
			{
				if (_Factory_* factory = _Factory_::GetInstance())
				{
					factory->template Unregister<_Type_>();
				}
			});
	}

	template<typename _Node_>
	void UIRegistrar::RegisterNode()
	{
		Register<NodeFactory, _Node_>();
	}

	template<typename _Layout_>
	void UIRegistrar::RegisterLayout()
	{
		Register<LayoutFactory, _Layout_>();
	}

	template<typename _LayoutParams_>
	void UIRegistrar::RegisterLayoutParams()
	{
		Register<LayoutParamsFactory, _LayoutParams_>();
	}

	template<typename _Interaction_>
	void UIRegistrar::RegisterInteraction()
	{
		Register<InteractionFactory, _Interaction_>();
	}

	template<typename _View_>
	void UIRegistrar::RegisterView()
	{
		Register<ViewFactory, _View_>();
	}
}

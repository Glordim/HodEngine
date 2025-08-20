#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Input.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API InputButton : public Input
	{
		REFLECTED_CLASS(InputButton, Input)

	public:

		InputButton(Identifier identifier, const String& displayName, const StateView& stateView);

		bool	ReadValue() const;
		bool	IsPressed() const;
		bool	IsJustPressed() const;

		String	ToString() const override;
	};
}

#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/Input.hpp"

namespace hod::input
{
	/// @brief 
	class HOD_INPUT_API InputAxis : public Input
	{
		REFLECTED_CLASS(InputAxis, Input)

	public:

		InputAxis() = default;
		InputAxis(Identifier identifier, const String& displayName, const StateView& stateView);

		float	ReadValue() const;

		String	ToString() const override;
	};
}

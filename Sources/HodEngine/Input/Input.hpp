#pragma once
#include "HodEngine/Input/Export.hpp"

#include "HodEngine/Input/StateView.hpp"

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>

namespace hod::input
{
	class Device;

	union Identifier
	{
		char _chars[8];
		uint64_t _value;
	};

	/// @brief 
	class HOD_INPUT_API Input
	{
		REFLECTED_CLASS_NO_PARENT(Input)

	public:

										Input(Identifier identifier, const String& displayName, const StateView& stateView);
										Input(const Input& other) = delete;
										Input(Input&& other) = delete;
										~Input() = default;

		Input&							operator = (const Input& other) = delete;
		Input&							operator = (Input&& other) = delete;

		Identifier						GetIdentifier() const;
		const String&					GetDisplayName() const;
		const StateView&				GetStateView() const;
		const State*					GetState() const;
		const State*					GetPreviousState() const;

		virtual String					ToString() const = 0;

		void							SetStatePtr(const State* state, const State* previousState);

	private:

		const Identifier				_identifier;
		String							_displayName;
		const StateView					_stateView;

		const State*					_state;
		const State*					_previousState;
	};
}

#pragma once
#include "HodEngine/Input/Export.hpp"

#include <string_view>
#include <HodEngine/Core/String.hpp>
#include <HodEngine/Core/Vector.hpp>
#include <HodEngine/Core/Concurency/SpinLock.hpp>
#include <HodEngine/Core/UID.hpp>

#include "HodEngine/Input/InputId.hpp"

namespace hod::input
{
	class Input;
	class Feedback;
	class State;

	/// @brief 
	class HOD_INPUT_API Device
	{
		friend class Api;

	public:

		enum class Type : uint8_t
		{
			PAD = 0,
			KEYBOARD,
			MOUSE,

			ANY,
		};

		enum Product
		{
			UNKNOWN = -1,
			XBOX, // Xbox360, XboxOne, XboxSerie
			DUALSHOCK_3,
			DUALSHOCK_4,
			DUALSENSE, // PS5
		};

	public:

								Device(Type type, const UID& uid, const std::string_view& name, Product product, uint32_t stateSize);
								Device(const Device&) = delete;
								Device(Device&&) = delete;
		virtual					~Device();

		Device&					operator = (const Device&) = delete;
		Device&					operator = (Device&&) = delete;

	public:

		bool					IsConnected() const;

		const UID&				GetUid() const;
		Type					GetType() const;
		Product					GetProduct() const;
		const String&			GetName() const;

		Input*					GetInput(InputId inputId) const;
		const Vector<Input*>&	GetInputs() const;
		
		void					UpdateState();
		
		virtual bool			ApplyFeedback(Feedback& feedback) = 0;
		
		uint32_t				GetStateSize() const;
		const State*			GetPreviousState() const;
		const State*			GetCurrentState() const;

	protected:

		template<typename _State_>
		_State_*				EditNextState() { return static_cast<_State_*>(EditNextState()); }
		State*					EditNextState();

		void					AddInput(Input* input);

		void					SetConnected(bool connected);
		void					SetProduct(Product product);
		void					SetName(const std::string_view& name);

		virtual void			ResetNextState() {};
		virtual void			MakeCurrent() = 0;
		inline void				MarkForCurrent() { _markForCurrent = true; }

	private:

		uint32_t				_stateSize = 0;
		SpinLock				_stateLock;
		State*					_previousState = nullptr;
		State*					_currentState = nullptr;
		State*					_nextState = nullptr;

		bool					_markForCurrent = false;

		String					_name;

		UID						_uid = UID::INVALID_UID;
		Type					_type = Type::ANY;
		Product					_product = Product::UNKNOWN;
		bool					_connected = false;

		Vector<Input*>			_inputs;
	};
}

#pragma once
#include "HodEngine/Input/Export.hpp"

#include <string_view>
#include "HodEngine/Core/String.hpp"
#include "HodEngine/Core/Vector.hpp"

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

								Device(Type type, const UID& uid, const std::string_view& name, Product product);
								Device(const Device&) = delete;
								Device(Device&&) = delete;
		virtual					~Device() = default;

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

		virtual const State*	GetState() const = 0;
		virtual uint32_t		GetStateSize() const = 0;

		virtual void			UpdateState() = 0;

		virtual bool			ApplyFeedback(Feedback& feedback) = 0;

	protected:

		void					AddInput(Input* input);

		void					SetConnected(bool connected);
		void					SetProduct(Product product);
		void					SetName(const std::string_view& name);

	private:

		std::string				_name;

		UID						_uid = UID::INVALID_UID;
		Type					_type = Type::ANY;
		Product					_product = Product::UNKNOWN;
		bool					_connected = false;

		Vector<Input*>			_inputs;
	};
}

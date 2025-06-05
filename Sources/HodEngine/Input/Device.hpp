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

	/// @brief 
	class HOD_INPUT_API Device
	{
		friend class Api;

	public:

		enum Type
		{
			ANY = -1,
			PAD = 0,
			KEYBOARD,
			MOUSE,
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
		const String&		GetName() const;

		Input*					GetInput(InputId inputId) const;
		const Vector<Input*>&	GetInputs() const;

		virtual bool			ApplyFeedback(Feedback& feedback) = 0;

		void					ReleaseAllInputs();

	protected:

		void					AddInput(Input* input);
		void					SetInputValue(Input& input, float value);

		void					ClearInputFlags();
		void					UpdateInputFlags();

		void					SetConnected(bool connected);
		void					SetProduct(Product product);
		void					SetName(const std::string_view& name);

	private:

		std::string				_name;

		UID						_uid = UID::INVALID_UID;
		Type					_type = Type::ANY;
		Product					_product = Product::UNKNOWN;
		bool					_connected = false;

		Vector<Input*>		_inputs;
	};
}

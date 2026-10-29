#include "HodEngine/UI2/Pch.hpp"
#include "HodEngine/UI2/Interaction.hpp"

namespace hod::inline ui2
{
	DESCRIBE_REFLECTED_CLASS(Interaction, reflectionDescriptor)
	{
		(void)reflectionDescriptor; // base interface, no own serializable fields
	}

	/// @brief
	/// @return
	Node* Interaction::GetNode() const
	{
		return _node;
	}

	/// @brief
	/// @param event
	/// @return
	bool Interaction::OnPointerEvent(const PointerEvent& event)
	{
		(void)event;
		return false;
	}

	/// @brief
	/// @param deltaTime
	void Interaction::Update(float deltaTime)
	{
		(void)deltaTime;
	}
}

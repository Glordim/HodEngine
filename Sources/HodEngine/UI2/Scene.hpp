#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline ui2
{
	class Node;

	class HOD_UI2_API Scene final
	{
	public:

		Scene();
		~Scene();

		void	AddChild(Node* node);
		void	RemoveChild(Node* node);

	private:

		Node*	_rootNode;
		Vector2 _designResolution = Vector2(1920.0f, 1080.0f);
	};
}

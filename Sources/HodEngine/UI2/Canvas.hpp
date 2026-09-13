#pragma once
#include "HodEngine/UI2/Export.hpp"

#include <HodEngine/Math/Vector2.hpp>

namespace hod::inline core
{
	class DocumentNode;
}

namespace hod::inline ui2
{
	class Node;
	class LayoutParams;

	class HOD_UI2_API Canvas final
	{
	public:

		Canvas();
		Canvas(const Canvas&) = delete;
		Canvas(Canvas&&) = delete;
		~Canvas();

		Canvas& operator=(const Canvas&) = delete;
		Canvas& operator=(Canvas&&) = delete;

		void	AddChild(Node* node);
		void	AddChild(Node* node, LayoutParams* layoutParams);
		void	RemoveChild(Node* node);

		Node*	GetRootNode() const;

		bool	SerializeInDocument(DocumentNode& documentNode);
		bool	DeserializeFromDocument(const DocumentNode& documentNode);

	private:

		Node*	_rootNode;
		Vector2 _designResolution = Vector2(1920.0f, 1080.0f);
	};
}

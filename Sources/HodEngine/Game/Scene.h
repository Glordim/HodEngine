#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.h>
#include <HodEngine/Core/Document/Document.h>

namespace hod::game
{
	/// @brief 
	class Scene
	{
		REFLECTED_CLASS_NO_PARENT(Scene)

	public:
						Scene() = default;
						Scene(const Scene&) = delete;
						Scene(Scene&&) = delete;
		virtual			~Scene() = default;

		Scene&			operator=(const Scene&) = delete;
		Scene&			operator=(Scene&&) = delete;

	public:

		bool			SerializeInDocument(Document::Node& documentNode) const;
		bool			DeserializeFromDocument(const Document::Node& documentNode);

	private:

		Document		_document;
	};
}

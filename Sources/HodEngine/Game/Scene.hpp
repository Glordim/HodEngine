#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

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

		const Document&	GetDocument() const { return _document; }

	private:

		Document		_document;
	};
}

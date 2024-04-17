#pragma once

#include <HodEngine/Core/Reflection/ReflectionMacros.hpp>
#include <HodEngine/Core/Document/Document.hpp>

namespace hod::game
{
	/// @brief 
	class Prefab
	{
		REFLECTED_CLASS_NO_PARENT(Prefab)

	public:
						Prefab() = default;
						Prefab(const Prefab&) = delete;
						Prefab(Prefab&&) = delete;
		virtual			~Prefab() = default;

		Prefab&			operator=(const Prefab&) = delete;
		Prefab&			operator=(Prefab&&) = delete;

	public:

		bool			SerializeInDocument(Document::Node& documentNode) const;
		bool			DeserializeFromDocument(const Document::Node& documentNode);

		const Document&	GetDocument() const { return _document; }

	private:

		Document		_document;
	};
}

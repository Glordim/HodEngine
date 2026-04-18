#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"

#include "HodEngine/Core/Hash.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"

namespace hod::inline core
{
	DESCRIBE_REFLECTED_CLASS(Document, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
			[](const void* instance, DocumentNode& documentNode)
			{
				String name = documentNode.GetName();
				documentNode.Copy(static_cast<const Document*>(instance)->GetRootNode());
				documentNode.SetName(name);
				return true;
			},
			[](void* instance, const DocumentNode& documentNode)
			{
				static_cast<const Document*>(instance)->GetRootNode().Copy(documentNode);
				static_cast<const Document*>(instance)->GetRootNode().SetName("");
				return true;
			});
	}

	/// @brief
	/// @param right
	/// @return
	Document& Document::operator=(const Document& right)
	{
		_root.Copy(right._root);
		return *this;
	}

	/// @brief
	/// @return
	DocumentNode& Document::GetRootNode() const
	{
		return const_cast<DocumentNode&>(_root);
	}

	/// @brief
	/// @param str
	/// @return
	Document::StringId Document::AddString(const std::string_view& str)
	{
		StringId hash = Hash::CompilationTimeFnv64(str);
		_stringTable[hash] = str;
		return hash;
	}

	/// @brief
	/// @param hash
	/// @return
	const String& Document::GetString(StringId hash)
	{
		return _stringTable[hash];
	}
}

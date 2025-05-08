#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/Document.hpp"

#include "HodEngine/Core/Hash.hpp"

#include "HodEngine/Core/Reflection/Traits/ReflectionTraitCustomSerialization.hpp"

namespace hod
{
	DESCRIBE_REFLECTED_CLASS(Document, reflectionDescriptor)
	{
		reflectionDescriptor.AddTrait<ReflectionTraitCustomSerialization>(
		[](const void* instance, Document::Node& documentNode)
		{
			std::string name = documentNode.GetName();
			documentNode.Copy(static_cast<const Document*>(instance)->GetRootNode());
			documentNode.SetName(name);
		},
		[](void* instance, const Document::Node& documentNode)
		{
			static_cast<const Document*>(instance)->GetRootNode().Copy(documentNode);
			static_cast<const Document*>(instance)->GetRootNode().SetName("");
		});
	}

	/// @brief 
	/// @param name 
	Document::Node::Node(Document& document, const std::string_view& name)
		: _document(document)
		, _type(Type::Object)
		, _name(name)
	{

	}

	/// @brief 
	Document::Node::~Node()
	{
		Document::Node* child = _firstChild;
		while (child != nullptr)
		{
			Document::Node* nextChild = child->_nextSibling;
			delete child;
			child = nextChild;
		}
	}

	/// @brief 
	/// @param node 
	void Document::Node::Detach()
	{
		if (_parent != nullptr)
		{
			_parent->Detach(*this);
		}
	}

	/// @brief 
	/// @param node 
	void Document::Node::Detach(Node& node)
	{
		Node* child = _firstChild;
		while (child != nullptr)
		{
			if (child == &node)
			{
				child = node._nextSibling;
				node._parent = nullptr;
				node._nextSibling = nullptr;
				return;
			}
			child = child->_nextSibling;
		}
	}

	/// @brief 
	/// @param node 
	void Document::Node::Attach(Node& node)
	{
		node.Detach();
		node._parent = this;

		if (_firstChild == nullptr)
		{
			_firstChild = &node;
		}
		else
		{
			Node* child = _firstChild;
			while (child->_nextSibling != nullptr)
			{
				child = child->_nextSibling;
			}
			child->_nextSibling = &node;
		}
	}

	/// @brief 
	/// @param name 
	/// @return 
	Document::Node& Document::Node::AddChild(const std::string_view& name)
	{
		// TODO check if an other child if same name exist

		Node* node = new Node(_document, name);
		if (node->GetName().empty() == true)
		{
			_type = Type::Array;
		}
		Attach(*node);
		return *node;
	}

	/// @brief 
	/// @param name 
	/// @return 
	const Document::Node* Document::Node::GetChild(const std::string_view& name) const
	{
		return FindChild(name);
	}

	/// @brief 
	/// @param name 
	/// @return 
	Document::Node& Document::Node::GetOrAddChild(const std::string_view& name)
	{
		// TODO check if an other child if same name exist

		Node* node = new Node(_document, name);
		Attach(*node);
		return *node;
	}

	/// @brief 
	/// @param name 
	/// @return 
	Document::Node& Document::Node::operator [] (const std::string_view& name)
	{
		return GetOrAddChild(name);
	}

	/// @brief 
	/// @return 
	Document::Node::Type Document::Node::GetType() const
	{
		return _type;
	}

	/// @brief 
	/// @param type 
	void Document::Node::SetTye(Type type)
	{
		// TODO verify if _type is unset otherwise the value will be corrupted
		_type = type;
	}

	/// @brief 
	/// @param name 
	void Document::Node::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief 
	/// @return 
	const std::string& Document::Node::GetName() const
	{
		return _name;
	}

	/// @brief 
	/// @return 
	Document::Node* Document::Node::GetFirstChild() const
	{
		return _firstChild;
	}

	/// @brief 
	/// @return 
	Document::Node* Document::Node::GetNextSibling() const
	{
		return _nextSibling;
	}

	/// @brief 
	/// @return 
	Document::Node* Document::Node::GetParent() const
	{
		return _parent;
	}

	/// @brief 
	/// @return 
	uint32_t  Document::Node::GetChildCount() const
	{
		uint32_t count = 0;

		Document::Node* child = _firstChild;
		while (child != nullptr)
		{
			++count;
			child = child->_nextSibling;
		}
		return count;
	}

	/// @brief 
	/// @param name 
	/// @return 
	Document::Node* Document::Node::FindChild(const std::string_view& name) const
	{
		Node* child = _firstChild;
		while (child != nullptr)
		{
			if (child->_name == name)
			{
				return child;
			}
			child = child->_nextSibling;
		}

		return nullptr;
	}

	void Document::Node::SetBool(bool value)
	{
		_type = Type::Bool;
		_value._bool = value;
	}

	void Document::Node::SetInt8(int8_t value)
	{
		_type = Type::Int8;
		_value._sint8 = value;
	}

	void Document::Node::SetInt16(int16_t value)
	{
		_type = Type::Int16;
		_value._sint16 = value;
	}

	void Document::Node::SetInt32(int32_t value)
	{
		_type = Type::Int32;
		_value._sint32 = value;
	}

	void Document::Node::SetInt64(int64_t value)
	{
		_type = Type::Int64;
		_value._sint64 = value;
	}

	void Document::Node::SetUInt8(uint8_t value)
	{
		_type = Type::UInt8;
		_value._uint8 = value;
	}

	void Document::Node::SetUInt16(uint16_t value)
	{
		_type = Type::UInt16;
		_value._uint16 = value;
	}

	void Document::Node::SetUInt32(uint32_t value)
	{
		_type = Type::UInt32;
		_value._uint32 = value;
	}

	void Document::Node::SetUInt64(uint64_t value)
	{
		_type = Type::UInt64;
		_value._uint64 = value;
	}

	void Document::Node::SetFloat32(float value)
	{
		_type = Type::Float32;
		_value._float32 = value;
	}

	void Document::Node::SetFloat64(double value)
	{
		_type = Type::Float64;
		_value._float64 = value;
	}

	void Document::Node::SetString(const std::string_view& value)
	{
		_type = Type::String;
		_value._stringId = _document.AddString(value);
	}

	bool Document::Node::GetBool() const
	{
		return _value._bool;
	}

	int8_t Document::Node::GetInt8() const
	{
		return _value._sint8;
	}

	int16_t Document::Node::GetInt16() const
	{
		return _value._sint16;
	}

	int32_t Document::Node::GetInt32() const
	{
		return _value._sint32;
	}

	int64_t Document::Node::GetInt64() const
	{
		return _value._sint64;
	}

	uint8_t Document::Node::GetUInt8() const
	{
		return _value._uint8;
	}

	uint16_t Document::Node::GetUInt16() const
	{
		return _value._uint16;
	}

	uint32_t Document::Node::GetUInt32() const
	{
		return _value._uint32;
	}

	uint64_t Document::Node::GetUInt64() const
	{
		return _value._uint64;
	}

	float Document::Node::GetFloat32() const
	{
		if (_type == Type::Float64)
		{
			return static_cast<float>(_value._float64); // todo
		}
		return _value._float32;
	}

	double Document::Node::GetFloat64() const
	{
		return _value._float64;
	}
	
	const std::string& Document::Node::GetString() const
	{
		// TODO add assert if doesn't match with type
		return _document.GetString(_value._stringId);
	}

	/// @brief 
	/// @param right 
	/// @return 
	Document& Document::operator = (const Document& right)
	{
		_root.Copy(right._root);
		return *this;
	}

	/// @brief 
	/// @param source 
	void Document::Node::Copy(const Document::Node& source)
	{
		Clear();

		Document::Node* sourceChild = source._firstChild;
		while (sourceChild != nullptr)
		{
			Document::Node& child = AddChild("");
			child.Copy(*sourceChild);

			sourceChild = sourceChild->_nextSibling;
		}

		_name = source._name;
		_type = source._type;

		if (_type == Type::String)
		{
			SetString(source.GetString());
		}
		else
		{
			_value = source._value;
		}
	}

	/// @brief 
	void Document::Node::Clear()
	{
		Document::Node* child = _firstChild;
		_firstChild = nullptr;

		while (child != nullptr)
		{
			Document::Node* nextChild = child->_nextSibling;
			delete child;
			child = nextChild;
		}
	}

	/// @brief 
	/// @return 
	Document::Node& Document::GetRootNode() const
	{
		return const_cast<Document::Node&>(_root);
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
	const std::string& Document::GetString(StringId hash)
	{
		return _stringTable[hash];
	}
}

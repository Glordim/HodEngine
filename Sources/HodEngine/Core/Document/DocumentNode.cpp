#include "HodEngine/Core/Pch.hpp"
#include "HodEngine/Core/Document/DocumentNode.hpp"
#include "HodEngine/Core/Document/Document.hpp"

namespace hod::inline core
{
	/// @brief
	/// @param name
	DocumentNode::DocumentNode(Document& document, const std::string_view& name)
	: _document(document)
	, _type(Type::Object)
	, _name(name)
	{
	}

	/// @brief
	DocumentNode::~DocumentNode()
	{
		DocumentNode* child = _firstChild;
		while (child != nullptr)
		{
			DocumentNode* nextChild = child->_nextSibling;
			DefaultAllocator::GetInstance().Delete(child);
			child = nextChild;
		}
	}

	/// @brief
	void DocumentNode::Detach()
	{
		if (_parent != nullptr)
		{
			_parent->Detach(*this);
		}
	}

	/// @brief
	/// @param node
	void DocumentNode::Detach(DocumentNode& node)
	{
		DocumentNode* prev = nullptr;
		DocumentNode* child = _firstChild;
		while (child != nullptr)
		{
			if (child == &node)
			{
				if (prev == nullptr)
					_firstChild = node._nextSibling;
				else
					prev->_nextSibling = node._nextSibling;
				node._parent = nullptr;
				node._nextSibling = nullptr;
				return;
			}
			prev = child;
			child = child->_nextSibling;
		}
	}

	/// @brief
	/// @param node
	void DocumentNode::Attach(DocumentNode& node)
	{
		node.Detach();
		node._parent = this;

		if (_firstChild == nullptr)
		{
			_firstChild = &node;
		}
		else
		{
			DocumentNode* child = _firstChild;
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
	DocumentNode& DocumentNode::AddChild(const std::string_view& name)
	{
		// TODO check if an other child if same name exist

		DocumentNode* node = DefaultAllocator::GetInstance().New<DocumentNode>(_document, name);
		if (node->GetName().Empty() == true)
		{
			_type = Type::Array;
		}
		Attach(*node);
		return *node;
	}

	/// @brief
	/// @param name
	/// @return
	const DocumentNode* DocumentNode::GetChild(const std::string_view& name) const
	{
		return FindChild(name);
	}

	/// @brief
	/// @param name
	/// @return
	DocumentNode& DocumentNode::GetOrAddChild(const std::string_view& name)
	{
		DocumentNode* existing = FindChild(name);
		if (existing != nullptr)
		{
			return *existing;
		}

		DocumentNode* node = DefaultAllocator::GetInstance().New<DocumentNode>(_document, name);
		Attach(*node);
		return *node;
	}

	/// @brief
	/// @param name
	/// @return
	DocumentNode& DocumentNode::operator[](const std::string_view& name)
	{
		return GetOrAddChild(name);
	}

	/// @brief
	/// @return
	DocumentNode::Type DocumentNode::GetType() const
	{
		return _type;
	}

	/// @brief
	/// @param type
	void DocumentNode::SetType(Type type)
	{
		// TODO verify if _type is unset otherwise the value will be corrupted
		_type = type;
	}

	/// @brief
	/// @param name
	void DocumentNode::SetName(const std::string_view& name)
	{
		_name = name;
	}

	/// @brief
	/// @return
	const String& DocumentNode::GetName() const
	{
		return _name;
	}

	/// @brief
	/// @return
	DocumentNode* DocumentNode::GetFirstChild() const
	{
		return _firstChild;
	}

	/// @brief
	/// @return
	DocumentNode* DocumentNode::GetNextSibling() const
	{
		return _nextSibling;
	}

	/// @brief
	/// @return
	DocumentNode* DocumentNode::GetParent() const
	{
		return _parent;
	}

	/// @brief
	/// @return
	uint32_t DocumentNode::GetChildCount() const
	{
		uint32_t count = 0;

		DocumentNode* child = _firstChild;
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
	DocumentNode* DocumentNode::FindChild(const std::string_view& name) const
	{
		DocumentNode* child = _firstChild;
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

	void DocumentNode::SetBool(bool value)
	{
		_type = Type::Bool;
		_value._bool = value;
	}

	void DocumentNode::SetInt8(int8_t value)
	{
		_type = Type::Int8;
		_value._sint8 = value;
	}

	void DocumentNode::SetInt16(int16_t value)
	{
		_type = Type::Int16;
		_value._sint16 = value;
	}

	void DocumentNode::SetInt32(int32_t value)
	{
		_type = Type::Int32;
		_value._sint32 = value;
	}

	void DocumentNode::SetInt64(int64_t value)
	{
		_type = Type::Int64;
		_value._sint64 = value;
	}

	void DocumentNode::SetUInt8(uint8_t value)
	{
		_type = Type::UInt8;
		_value._uint8 = value;
	}

	void DocumentNode::SetUInt16(uint16_t value)
	{
		_type = Type::UInt16;
		_value._uint16 = value;
	}

	void DocumentNode::SetUInt32(uint32_t value)
	{
		_type = Type::UInt32;
		_value._uint32 = value;
	}

	void DocumentNode::SetUInt64(uint64_t value)
	{
		_type = Type::UInt64;
		_value._uint64 = value;
	}

	void DocumentNode::SetFloat32(float value)
	{
		_type = Type::Float32;
		_value._float32 = value;
	}

	void DocumentNode::SetFloat64(double value)
	{
		_type = Type::Float64;
		_value._float64 = value;
	}

	void DocumentNode::SetString(const std::string_view& value)
	{
		_type = Type::String;
		_value._stringHash = _document.AddString(value);
	}

	bool DocumentNode::GetBool() const
	{
		return _value._bool;
	}

	int8_t DocumentNode::GetInt8() const
	{
		return _value._sint8;
	}

	int16_t DocumentNode::GetInt16() const
	{
		return _value._sint16;
	}

	int32_t DocumentNode::GetInt32() const
	{
		return _value._sint32;
	}

	int64_t DocumentNode::GetInt64() const
	{
		return _value._sint64;
	}

	uint8_t DocumentNode::GetUInt8() const
	{
		return _value._uint8;
	}

	uint16_t DocumentNode::GetUInt16() const
	{
		return _value._uint16;
	}

	uint32_t DocumentNode::GetUInt32() const
	{
		return _value._uint32;
	}

	uint64_t DocumentNode::GetUInt64() const
	{
		return _value._uint64;
	}

	float DocumentNode::GetFloat32() const
	{
		if (_type == Type::Float64)
		{
			return static_cast<float>(_value._float64); // todo
		}
		return _value._float32;
	}

	double DocumentNode::GetFloat64() const
	{
		return _value._float64;
	}

	const String& DocumentNode::GetString() const
	{
		// TODO add assert if doesn't match with type
		return _document.GetString(_value._stringHash);
	}

	/// @brief
	/// @param source
	void DocumentNode::Copy(const DocumentNode& source)
	{
		Clear();

		DocumentNode* sourceChild = source._firstChild;
		while (sourceChild != nullptr)
		{
			DocumentNode& child = AddChild("");
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
	void DocumentNode::Clear()
	{
		DocumentNode* child = _firstChild;
		_firstChild = nullptr;

		while (child != nullptr)
		{
			DocumentNode* nextChild = child->_nextSibling;
			DefaultAllocator::GetInstance().Delete(child);
			child = nextChild;
		}
	}
}

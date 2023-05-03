#include "HodEngine/Core/Document/Document.h"

namespace hod
{
	namespace core
	{
		/// @brief 
		/// @param name 
		Document::Node::Node(const std::string_view& name)
			: _name(name)
		{

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

			Node* node = new Node(name);
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

			Node* node = new Node(name);
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
			_value._bool = value;
		}

		void Document::Node::SetInt8(int8_t value)
		{
			_value._sint8 = value;
		}

		void Document::Node::SetInt16(int16_t value)
		{
			_value._sint16 = value;
		}

		void Document::Node::SetInt32(int32_t value)
		{
			_value._sint32 = value;
		}

		void Document::Node::SetInt64(int64_t value)
		{
			_value._sint64 = value;
		}

		void Document::Node::SetUInt8(uint8_t value)
		{
			_value._uint8 = value;
		}

		void Document::Node::SetUInt16(uint16_t value)
		{
			_value._uint16 = value;
		}

		void Document::Node::SetUInt32(uint32_t value)
		{
			_value._uint32 = value;
		}

		void Document::Node::SetUInt64(uint64_t value)
		{
			_value._uint64 = value;
		}

		void Document::Node::SetFloat32(float value)
		{
			_value._float32 = value;
		}

		void Document::Node::SetFloat64(double value)
		{
			_value._float64 = value;
		}

		void Document::Node::SetString(const std::string_view& value)
		{
			_value._string = value;
		}

		template<>
		void Document::Node::SetValue(const bool& value)
		{
			SetBool(value);
		}

		template<>
		void Document::Node::SetValue(const int8_t& value)
		{
			SetInt8(value);
		}

		template<>
		void Document::Node::SetValue(const int16_t& value)
		{
			SetInt16(value);
		}

		template<>
		void Document::Node::SetValue(const int32_t& value)
		{
			SetInt32(value);
		}

		template<>
		void Document::Node::SetValue(const int64_t& value)
		{
			SetInt64(value);
		}

		template<>
		void Document::Node::SetValue(const uint8_t& value)
		{
			SetUInt8(value);
		}

		template<>
		void Document::Node::SetValue(const uint16_t& value)
		{
			SetUInt16(value);
		}

		template<>
		void Document::Node::SetValue(const uint32_t& value)
		{
			SetUInt32(value);
		}

		template<>
		void Document::Node::SetValue(const uint64_t& value)
		{
			SetUInt64(value);
		}

		template<>
		void Document::Node::SetValue(const float& value)
		{
			SetFloat32(value);
		}

		template<>
		void Document::Node::SetValue(const double& value)
		{
			SetFloat64(value);
		}

		template<>
		void Document::Node::SetValue(const std::string_view& value)
		{
			SetString(value);
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
			return _value._float32;
		}

		double Document::Node::GetFloat64() const
		{
			return _value._float64;
		}

		const std::string_view& Document::Node::GetString() const
		{
			return _value._string;
		}

		/// @brief 
		/// @return 
		Document::Node& Document::GetRootNode() const
		{
			return const_cast<Document::Node&>(_root);
		}
	}
}

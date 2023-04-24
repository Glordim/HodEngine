#include "Document.h"

namespace hod
{
	namespace core
	{
		/// @brief 
		/// @param name 
		Document::Element::Element(const char* name)
			: _name(name)
		{

		}

		/// @brief 
		/// @param name 
		Document::Element::Element(const char* name, uint32_t size)
			: _name(name, size)
		{

		}

		/// @brief 
		/// @param element 
		void Document::Element::Detach()
		{
			if (_parent != nullptr)
			{
				_parent->Detach(*this);
			}
		}

		/// @brief 
		/// @param element 
		void Document::Element::Detach(Element& element)
		{
			Element* child = _firstChild;
			while (child != nullptr)
			{
				if (child == &element)
				{
					child = element._nextSibling;
					element._parent = nullptr;
					element._nextSibling = nullptr;
					return;
				}
				child = child->_nextSibling;
			}
		}

		/// @brief 
		/// @param element 
		void Document::Element::Attach(Element& element)
		{
			element.Detach();
			element._parent = this;

			if (_firstChild == nullptr)
			{
				_firstChild = &element;
			}
			else
			{
				Element* child = _firstChild;
				while (child->_nextSibling != nullptr)
				{
					child = child->_nextSibling;
				}
				child->_nextSibling = &element;
			}
		}

		/// @brief 
		/// @param key 
		/// @return 
		Document::Element& Document::Element::AddChild(const char* key)
		{
			// TODO check if an other child if same name exist

			Element* element = new Element(key);
			Attach(*element);
		}

		/// @brief 
		/// @param key 
		/// @param size 
		/// @return 
		Document::Element& Document::Element::AddChild(const char* key, uint32_t size)
		{
			// TODO check if an other child if same name exist

			Element* element = new Element(key, size);
			Attach(*element);
		}

		/// @brief 
		/// @return 
		Document::Element* Document::Element::GetFirstChild() const
		{
			return _firstChild;
		}

		/// @brief 
		/// @return 
		Document::Element* Document::Element::GetNextSibling() const
		{
			return _nextSibling;
		}

		/// @brief 
		/// @return 
		Document::Element* Document::Element::GetParent() const
		{
			return _parent;
		}

		/// @brief 
		/// @param key 
		/// @return 
		Document::Element* Document::Element::FindChild(const char* key) const
		{
			Element* child = _firstChild;
			while (child != nullptr)
			{
				if (std::strcmp(child->_name, key) == 0)
				{
					return child;
				}
				child = child->_nextSibling;
			}

			return nullptr;
		}

		void Document::Element::SetBool(const char* key, bool value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._bool = value;
			}
		}

		void Document::Element::SetInt8(const char* key, int8_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._sint8 = value;
			}
		}

		void Document::Element::SetInt16(const char* key, int16_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._sint16 = value;
			}
		}

		void Document::Element::SetInt32(const char* key, int32_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._sint32 = value;
			}
		}

		void Document::Element::SetInt64(const char* key, int64_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._sint64 = value;
			}
		}

		void Document::Element::SetUInt8(const char* key, uint8_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._uint8 = value;
			}
		}

		void Document::Element::SetUInt16(const char* key, uint16_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._uint16 = value;
			}
		}

		void Document::Element::SetUInt32(const char* key, uint32_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._uint32 = value;
			}
		}

		void Document::Element::SetUInt64(const char* key, uint64_t value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._uint64 = value;
			}
		}

		void Document::Element::SetFloat32(const char* key, float value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._float32 = value;
			}
		}

		void Document::Element::SetFloat64(const char* key, double value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._float64 = value;
			}
		}

		void Document::Element::SetString(const char* key, const char* value)
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				child->_value._string = value;
			}
		}

		bool Document::Element::GetBool(const char* key, bool defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._bool;
			}
			return defaultValue;
		}

		int8_t Document::Element::GetInt8(const char* key, int8_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._sint8;
			}
			return defaultValue;
		}

		int16_t Document::Element::GetInt16(const char* key, int16_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._sint16;
			}
			return defaultValue;
		}

		int32_t Document::Element::GetInt32(const char* key, int32_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._sint32;
			}
			return defaultValue;
		}

		int64_t Document::Element::GetInt64(const char* key, int64_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._sint64;
			}
			return defaultValue;
		}

		uint8_t Document::Element::GetUInt8(const char* key, uint8_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._uint8;
			}
			return defaultValue;
		}

		uint16_t Document::Element::GetUInt16(const char* key, uint16_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._uint16;
			}
			return defaultValue;
		}

		uint32_t Document::Element::GetUInt32(const char* key, uint32_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._uint32;
			}
			return defaultValue;
		}

		uint64_t Document::Element::GetUInt64(const char* key, uint64_t defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._uint64;
			}
			return defaultValue;
		}

		float Document::Element::GetFloat32(const char* key, float defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._float32;
			}
			return defaultValue;
		}

		double Document::Element::GetFloat64(const char* key, double defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._float64;
			}
			return defaultValue;
		}

		const char* Document::Element::GetString(const char* key, const char* defaultValue) const
		{
			Element* child = FindChild(key);
			if (child != nullptr)
			{
				return child->_value._string;
			}
			return defaultValue;
		}

		/// @brief 
		/// @return 
		Document::Element& Document::GetRootElement() const
		{
			return const_cast<Document::Element&>(_root);
		}
	}
}

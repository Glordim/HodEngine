#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstdint>
#include <span>
#include <string_view>

namespace hod::inline core
{
	class Document;

	class HOD_CORE_API DocumentNode
	{
		friend class Document;
		friend class Allocator;

	public:
		enum class Type : uint8_t
		{
			Object,
			Array,
			Bool,
			Int8,
			Int16,
			Int32,
			Int64,
			UInt8,
			UInt16,
			UInt32,
			UInt64,
			Float32,
			Float64,
			String,
		};

	public:
		DocumentNode&       AddChild(const std::string_view& name);
		const DocumentNode* GetChild(const std::string_view& name) const;
		DocumentNode&       GetOrAddChild(const std::string_view& name);

		DocumentNode* GetFirstChild() const;
		DocumentNode* GetNextSibling() const;
		DocumentNode* GetParent() const;

		uint32_t GetChildCount() const;

		void          SetName(const std::string_view& name);
		const String& GetName() const;

		Type GetType() const;
		void SetType(Type type);

		bool IsValid() const;
		bool IsObject() const;
		bool IsArray() const;

		template<typename T, size_t Size>
		void SetValues(const std::span<T, Size>& values);

		template<typename T>
		void SetValue(const T& value);
		void SetBool(bool value);
		void SetInt8(int8_t value);
		void SetInt16(int16_t value);
		void SetInt32(int32_t value);
		void SetInt64(int64_t value);
		void SetUInt8(uint8_t value);
		void SetUInt16(uint16_t value);
		void SetUInt32(uint32_t value);
		void SetUInt64(uint64_t value);
		void SetFloat32(float value);
		void SetFloat64(double value);
		void SetString(const std::string_view& value);

		bool          GetBool() const;
		int8_t        GetInt8() const;
		int16_t       GetInt16() const;
		int32_t       GetInt32() const;
		int64_t       GetInt64() const;
		uint8_t       GetUInt8() const;
		uint16_t      GetUInt16() const;
		uint32_t      GetUInt32() const;
		uint64_t      GetUInt64() const;
		float         GetFloat32() const;
		double        GetFloat64() const;
		const String& GetString() const;

		DocumentNode& operator[](const std::string_view& name);

		void Copy(const DocumentNode& source);
		void Clear();

	private:
		union Value
		{
			bool     _bool;
			int8_t   _sint8;
			int16_t  _sint16;
			int32_t  _sint32;
			int64_t  _sint64;
			uint8_t  _uint8;
			uint16_t _uint16;
			uint32_t _uint32;
			uint64_t _uint64;
			float    _float32;
			double   _float64;
			uint64_t _stringHash;
		};

	private:
		DocumentNode(Document& document, const std::string_view& name);
		DocumentNode(const DocumentNode&) = delete;
		DocumentNode(DocumentNode&&) = delete;
		~DocumentNode();

		DocumentNode& operator=(const DocumentNode&) = delete;
		DocumentNode& operator=(DocumentNode&&) = delete;

	private:
		void          Detach();
		void          Detach(DocumentNode& node);
		void          Attach(DocumentNode& node);
		DocumentNode* FindChild(const std::string_view& name) const;

	private:
		DocumentNode* _firstChild  = nullptr;
		DocumentNode* _lastChild   = nullptr;
		DocumentNode* _nextSibling = nullptr;
		DocumentNode* _parent      = nullptr;
		Document&     _document;

		Type   _type;
		String _name;
		Value  _value;
	};

	template<typename T, size_t Size>
	void DocumentNode::SetValues(const std::span<T, Size>& values)
	{
		// TODO ensure no child
		_type = Type::Array;

		for (const T& value : values)
		{
			DocumentNode& child = AddChild("");
			child.SetValue(value);
		}
	}

	template<>
	inline void DocumentNode::SetValue(const bool& value)
	{
		SetBool(value);
	}

	template<>
	inline void DocumentNode::SetValue(const int8_t& value)
	{
		SetInt8(value);
	}

	template<>
	inline void DocumentNode::SetValue(const int16_t& value)
	{
		SetInt16(value);
	}

	template<>
	inline void DocumentNode::SetValue(const int32_t& value)
	{
		SetInt32(value);
	}

	template<>
	inline void DocumentNode::SetValue(const int64_t& value)
	{
		SetInt64(value);
	}

	template<>
	inline void DocumentNode::SetValue(const uint8_t& value)
	{
		SetUInt8(value);
	}

	template<>
	inline void DocumentNode::SetValue(const uint16_t& value)
	{
		SetUInt16(value);
	}

	template<>
	inline void DocumentNode::SetValue(const uint32_t& value)
	{
		SetUInt32(value);
	}

	template<>
	inline void DocumentNode::SetValue(const uint64_t& value)
	{
		SetUInt64(value);
	}

	template<>
	inline void DocumentNode::SetValue(const float& value)
	{
		SetFloat32(value);
	}

	template<>
	inline void DocumentNode::SetValue(const double& value)
	{
		SetFloat64(value);
	}

	template<>
	inline void DocumentNode::SetValue(const String& value)
	{
		SetString(value);
	}

	template<>
	inline void DocumentNode::SetValue(const std::string_view& value)
	{
		SetString(value);
	}
}

#pragma once
#include "HodEngine/Core/Export.hpp"

#include "HodEngine/Core/String.hpp"
#include <cstdint>
#include <span>
#include <string_view>

namespace hod
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
		DocumentNode* _nextSibling = nullptr;
		DocumentNode* _parent      = nullptr;
		Document&     _document;

		Type   _type;
		String _name;
		Value  _value;
	};
}

#include "DocumentNode.inl"

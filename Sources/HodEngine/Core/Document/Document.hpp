#pragma once
#include "HodEngine/Core/Export.hpp"

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <map>

#include "HodEngine/Core/Reflection/ReflectionMacros.hpp"

namespace hod
{
	/// @brief 
	class HOD_CORE_API Document
	{
		REFLECTED_CLASS_NO_VIRTUAL(Document)

	private:

		using StringId = uint64_t;

	public:

		class HOD_CORE_API Node
		{
			friend class Document;

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

			Node&		AddChild(const std::string_view& name);
			const Node*	GetChild(const std::string_view& name) const;
			Node&		GetOrAddChild(const std::string_view& name);

			Node*		GetFirstChild() const;
			Node*		GetNextSibling() const;
			Node*		GetParent() const;

			uint32_t	GetChildCount() const;

			void				SetName(const std::string_view& name);
			const std::string&	GetName() const;

			Type 		GetType() const;
			void		SetTye(Type type);

			bool		IsValid() const;
			bool		IsObject() const;
			bool		IsArray() const;

			template<typename T, size_t Size>
			void		SetValues(const std::span<T, Size>& values);

			template<typename T>
			void		SetValue(const T& value);
			void		SetBool(bool value);
			void		SetInt8(int8_t value);
			void		SetInt16(int16_t value);
			void		SetInt32(int32_t value);
			void		SetInt64(int64_t value);
			void		SetUInt8(uint8_t value);
			void		SetUInt16(uint16_t value);
			void		SetUInt32(uint32_t value);
			void		SetUInt64(uint64_t value);
			void		SetFloat32(float value);
			void		SetFloat64(double value);
			void		SetString(const std::string_view& value);

			bool		GetBool() const;
			int8_t		GetInt8() const;
			int16_t		GetInt16() const;
			int32_t		GetInt32() const;
			int64_t		GetInt64() const;
			uint8_t		GetUInt8() const;
			uint16_t	GetUInt16() const;
			uint32_t	GetUInt32() const;
			uint64_t	GetUInt64() const;
			float		GetFloat32() const;
			double		GetFloat64() const;
			const std::string& GetString() const;

			Node&		operator [] (const std::string_view& name);

		private:

			union Value
			{
				bool				_bool;
				int8_t				_sint8;
				int16_t				_sint16;
				int32_t				_sint32;
				int64_t				_sint64;
				uint8_t				_uint8;
				uint16_t			_uint16;
				uint32_t			_uint32;
				uint64_t			_uint64;
				float				_float32;
				double				_float64;
				StringId			_stringId;
			};

		private:

						Node(Document& document, const std::string_view& name);
						Node(const Node&) = delete;
						Node(Node&&) = delete;
						~Node();

			Node&		operator = (const Node&) = delete;
			Node&		operator = (Node&&) = delete;

		public:

			void		Copy(const Node& source);
			void		Clear();

		private:

			void		Detach();
			void		Detach(Node& node);
			void		Attach(Node& node);
			Node*		FindChild(const std::string_view& name) const;

		private:

			Node*				_firstChild = nullptr;
			Node*				_nextSibling = nullptr;
			Node*				_parent = nullptr;
			Document&			_document;

			Type				_type;
			std::string			_name;
			Value				_value;
		};

	public:

					Document() = default;
					Document(const Document&) = delete;
					Document(Document&&) = delete;
					~Document() = default;

		Document&	operator = (const Document& right);
		Document&	operator = (Document&&) = delete;

	public:

		Node&	GetRootNode() const;

		/*
		bool		Parse(const char* buffer, uint32_t size);

		std::string	Dump() const;
		void		Dump(const char* buffer, uint32_t size) const;
		*/

	private:

		StringId			AddString(const std::string_view& str);
		const std::string&	GetString(StringId hash);

	private:

		Node		_root = Node(*this, "");

		std::map<uint64_t, std::string>	_stringTable;
	};
}

#include "Document.inl"

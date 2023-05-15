#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <map>

namespace hod
{
	namespace core
	{
		/// @brief 
		class Document
		{
		public:

			class Node
			{
				friend class Document;

			public:

				Node&		AddChild(const std::string_view& name);
				const Node*	GetChild(const std::string_view& name) const;
				Node&		GetOrAddChild(const std::string_view& name);

				Node*		GetFirstChild() const;
				Node*		GetNextSibling() const;
				Node*		GetParent() const;

				const char* GetName() const;
				bool		IsValid() const;
				bool		IsObject() const;
				bool		IsArray() const;

				Node*		AddArray(const char* name);

				template<typename T>
				void		SetValues(const std::span<const T>& values);

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
				void		SetString(const std::string& value);
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

				template<typename T>
				T*			GetValues() const;

				Node&		operator [] (const std::string_view& name);

			private:

				enum class Type
				{
					Object,
					Array,
					Value,
					String,
				};

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
				};

			private:

							Node(Document& document, const std::string_view& name);
							Node(const Node&) = delete;
							Node(Node&&) = delete;
							~Node() = default;

				Node&		operator = (const Node&) = delete;
				Node&		operator = (Node&&) = delete;

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

			Document&	operator = (const Document&) = delete;
			Document&	operator = (Document&&) = delete;

		public:

			Node&	GetRootNode() const;

			/*
			bool		Parse(const char* buffer, uint32_t size);

			std::string	Dump() const;
			void		Dump(const char* buffer, uint32_t size) const;
			*/

		private:

			uint64_t			AddString(const std::string_view& str);
			const std::string&	GetString(uint64_t hash);

		private:

			Node		_root = Node(*this, "Root");

			std::map<uint64_t, std::string>	_stringTable;
		};
	}
}

#include "Document.inl"

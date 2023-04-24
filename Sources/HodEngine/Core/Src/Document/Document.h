#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace hod
{
	namespace core
	{
		/// @brief 
		class Document
		{
		public:

			class Element
			{
				friend class Document;

			public:

				Element&	AddChild(const char* key);
				Element&	AddChild(const char* key, uint32_t size);

				Element*	GetFirstChild() const;
				Element*	GetNextSibling() const;
				Element*	GetParent() const;

				const char* GetName() const;
				bool		IsValid() const;
				bool		IsObject() const;
				bool		IsArray() const;

				Element*	AddArray(const char* name);

				template<typename T>
				void		SetValues(std::vector<T> values);

				template<typename T>
				void		SetValue(const char* key, T value);
				void		SetBool(const char* key, bool value);
				void		SetInt8(const char* key, int8_t value);
				void		SetInt16(const char* key, int16_t value);
				void		SetInt32(const char* key, int32_t value);
				void		SetInt64(const char* key, int64_t value);
				void		SetUInt8(const char* key, uint8_t value);
				void		SetUInt16(const char* key, uint16_t value);
				void		SetUInt32(const char* key, uint32_t value);
				void		SetUInt64(const char* key, uint64_t value);
				void		SetFloat32(const char* key, float value);
				void		SetFloat64(const char* key, double value);
				void		SetString(const char* key, const char* value);

				template<typename T>
				T			GetValue(const char* key, T defaultValue) const;
				bool		GetBool(const char* key, bool defaultValue) const;
				int8_t		GetInt8(const char* key, int8_t defaultValue) const;
				int16_t		GetInt16(const char* key, int16_t defaultValue) const;
				int32_t		GetInt32(const char* key, int32_t defaultValue) const;
				int64_t		GetInt64(const char* key, int64_t defaultValue) const;
				uint8_t		GetUInt8(const char* key, uint8_t defaultValue) const;
				uint16_t	GetUInt16(const char* key, uint16_t defaultValue) const;
				uint32_t	GetUInt32(const char* key, uint32_t defaultValue) const;
				uint64_t	GetUInt64(const char* key, uint64_t defaultValue) const;
				float		GetFloat32(const char* key, float defaultValue) const;
				double		GetFloat64(const char* key, double defaultValue) const;
				const char*	GetString(const char* key, const char* defaultValue) const;

				template<typename T>
				T*			GetValues() const;

			private:

				union Value
				{
					bool		_bool;
					int8_t		_sint8;
					int16_t		_sint16;
					int32_t		_sint32;
					int64_t		_sint64;
					uint8_t		_uint8;
					uint16_t	_uint16;
					uint32_t	_uint32;
					uint64_t	_uint64;
					float		_float32;
					double		_float64;
					const char* _string;
				};

			private:

							Element(const char* name);
							Element(const char* name, uint32_t size);
							Element(const Element&) = delete;
							Element(Element&&) = delete;
							~Element() = default;

				Element&	operator = (const Element&) = delete;
				Element&	operator = (Element&&) = delete;

			private:

				void		Detach();
				void		Detach(Element& element);
				void		Attach(Element& element);
				Element*	FindChild(const char* key) const;

			private:

				Element*	_firstChild = nullptr;
				Element*	_nextSibling = nullptr;
				Element*	_parent = nullptr;

				std::string	_name = nullptr;
				Value		_value;
			};

		public:

						Document() = default;
						Document(const Document&) = delete;
						Document(Document&&) = delete;
						~Document() = default;

			Document&	operator = (const Document&) = delete;
			Document&	operator = (Document&&) = delete;

		public:

			Element&	GetRootElement() const;

			/*
			bool		Parse(const char* buffer, uint32_t size);

			std::string	Dump() const;
			void		Dump(const char* buffer, uint32_t size) const;
			*/

		private:

			Element		_root = Element("Root");
		};
	}
}


namespace hod
{
	template<typename T, size_t Size>
	void Document::Node::SetValues(const std::span<T, Size>& values)
	{
		// TODO ensure no child
		_type = Type::Array;

		for (const T& value : values)
		{
			Node& child = AddChild("");
			child.SetValue(value);
		}
	}

	template<>
	inline void Document::Node::SetValue(const bool& value)
	{
		SetBool(value);
	}

	template<>
	inline void Document::Node::SetValue(const int8_t& value)
	{
		SetInt8(value);
	}

	template<>
	inline void Document::Node::SetValue(const int16_t& value)
	{
		SetInt16(value);
	}

	template<>
	inline void Document::Node::SetValue(const int32_t& value)
	{
		SetInt32(value);
	}

	template<>
	inline void Document::Node::SetValue(const int64_t& value)
	{
		SetInt64(value);
	}

	template<>
	inline void Document::Node::SetValue(const uint8_t& value)
	{
		SetUInt8(value);
	}

	template<>
	inline void Document::Node::SetValue(const uint16_t& value)
	{
		SetUInt16(value);
	}

	template<>
	inline void Document::Node::SetValue(const uint32_t& value)
	{
		SetUInt32(value);
	}

	template<>
	inline void Document::Node::SetValue(const uint64_t& value)
	{
		SetUInt64(value);
	}

	template<>
	inline void Document::Node::SetValue(const float& value)
	{
		SetFloat32(value);
	}

	template<>
	inline void Document::Node::SetValue(const double& value)
	{
		SetFloat64(value);
	}

	template<>
	inline void Document::Node::SetValue(const std::string& value)
	{
		SetString(value);
	}

	template<>
	inline void Document::Node::SetValue(const std::string_view& value)
	{
		SetString(value);
	}
}

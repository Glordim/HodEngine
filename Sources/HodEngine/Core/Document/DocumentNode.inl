
namespace hod::inline core
{
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
